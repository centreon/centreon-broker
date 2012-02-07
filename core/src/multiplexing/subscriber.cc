/*
** Copyright 2009-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <QMutexLocker>
#include <stdlib.h>
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/internal.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  subscriber is not copyable. Any attempt to use the copy constructor
 *  will result in a call to abort().
 *
 *  @param[in] s Unused.
 */
subscriber::subscriber(subscriber const& s) : io::stream(s) {
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  subscriber is not copyable. Any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  @param[in] s Unused.
 *
 *  @return This object.
 */
subscriber& subscriber::operator=(subscriber const& s) {
  (void)s;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Release all events stored within the internal list.
 */
void subscriber::clean() {
  QMutexLocker lock(&_mutex);
  _events.clear();
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
subscriber::subscriber() {
  // Register self in subscriber list.
  QMutexLocker lock1(&gl_subscribersm);
  QMutexLocker lock2(&_mutex);
  _process_in = true;
  _process_out = true;
  gl_subscribers.push_back(this);
  logging::debug << logging::LOW << "multiplexing: "
    << gl_subscribers.size()
    << " subscribers are registered after insertion";
}

/**
 *  Destructor.
 */
subscriber::~subscriber() {
  clean();
  QMutexLocker lock(&gl_subscribersm);
  for (QVector<subscriber*>::iterator it = gl_subscribers.begin();
       it != gl_subscribers.end();)
    if (*it == this)
      it = gl_subscribers.erase(it);
    else
      ++it;
}

/**
 *  Unregister or re-register from event publishing notifications.
 *
 *  @param[in] in  Process input events.
 *  @param[in] out Process output events.
 */
void subscriber::process(bool in, bool out) {
  // Debug message.
  logging::debug(logging::low) << "multiplexing: subscriber " \
    "processing request in=" << in << ", out=" << out;

  // Lock mutex.
  QMutexLocker lock(&_mutex);

  // Set data.
  _process_in = in;
  _process_out = (_process_out && out);

  // Unregister.
  if (!in || !out) {
    _cv.wakeAll();
    // Log message.
    logging::debug(logging::low) << "multiplexing: "
      << gl_subscribers.size()
      << " subscribers are registered after deletion";
  }
  // Re-register.
  else if (in && out) {
    // Log message.
    logging::debug(logging::low) << "multiplexing: "
      << gl_subscribers.size()
      << " subscribers are registered after reregistration";
  }

  return ;
}

/**
 *  Get the next available event.
 *
 *  @return Next available event.
 */
QSharedPointer<io::data> subscriber::read() {
  return (this->read(-1));
}

/**
 *  Get the next available event without waiting after deadline.
 *
 *  @param[in] deadline Date that shouldn't be exceeded while waiting
 *                      for a new event.
 *
 *  @return Next available event, NULL if timeout occured.
 */
QSharedPointer<io::data> subscriber::read(time_t deadline) {
  QSharedPointer<io::data> event;
  QMutexLocker lock(&_mutex);

  // No data is directly available.
  if (_events.empty()) {
    // Wait a while if subscriber was not shutdown.
    if (_process_in && _process_out) {
      if (-1 == deadline)
        _cv.wait(&_mutex);
      else
        _cv.wait(&_mutex, deadline);
      if (!_events.isEmpty()) {
        event = _events.dequeue();
        logging::debug(logging::low) << "multiplexing: "
          << _events.size() << " events remaining in subcriber";
      }
    }
    // If subscriber is shutdown, notify caller.
    else
      throw (io::exceptions::shutdown(true, true) << "subscriber "
               << this << " is shutdown, cannot read");
  }
  // Data is available, no need to wait.
  else {
    if (!_process_in && _process_out)
      throw (io::exceptions::shutdown(true, false) << "subscriber "
             << this << " is shutdown, cannot read");
    event = _events.dequeue();
    logging::debug(logging::low) << "multiplexing: " << _events.size()
      << " events remaining in subscriber";
  }
  return (event);
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 */
void subscriber::write(QSharedPointer<io::data> event) {
  QMutexLocker lock(&_mutex);
  _events.enqueue(event);
  _cv.wakeOne();
  return ;
}
