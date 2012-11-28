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

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/internal.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

unsigned int subscriber::_event_queue_max_size = std::numeric_limits<unsigned int>::max();

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] temporary Temporary stream to write data when memory
 *                       queue is full.
 */
subscriber::subscriber(io::endpoint const* temporary)
  : _total_events(0) {
  // Register self in subscriber list.
  QMutexLocker lock1(&gl_subscribersm);
  QMutexLocker lock2(&_mutex);
  _process_in = true;
  _process_out = true;
  if (temporary) {
    _endp_temporary = std::auto_ptr<io::endpoint>(temporary->clone());
    _temporary = _endp_temporary->open();
  }
  gl_subscribers.push_back(this);
  logging::debug(logging::low) << "multiplexing: "
    << gl_subscribers.size()
    << " subscribers are registered after insertion";
}

/**
 *  Destructor.
 */
subscriber::~subscriber() {
  clean();
  QMutexLocker lock(&gl_subscribersm);
  for (QVector<subscriber*>::iterator it(gl_subscribers.begin());
       it != gl_subscribers.end();)
    if (*it == this)
      it = gl_subscribers.erase(it);
    else
      ++it;
}

/**
 *  Set the maximume event queue size.
 *
 *  @param[in] max The size limit.
 */
void subscriber::event_queue_max_size(unsigned int max) throw () {
  if (!max)
    max = std::numeric_limits<unsigned int>::max();
  _event_queue_max_size = max;
}

/**
 *  Get the maximum event queue size.
 *
 *  @return The size limit.
 */
unsigned int subscriber::event_queue_max_size() throw () {
  return (_event_queue_max_size);
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
 *  @param[out] d Next available event.
 */
void subscriber::read(misc::shared_ptr<io::data>& d) {
  this->read(d, (time_t)-1);
  return ;
}

/**
 *  Get the next available event without waiting more than timeout.
 *
 *  @param[out] event     Next available event.
 *  @param[in]  timeout   Date limit.
 *  @param[out] timed_out Set to true if read timed out.
 */
void subscriber::read(
                   misc::shared_ptr<io::data>& event,
                   time_t timeout,
                   bool* timed_out) {
  QMutexLocker lock(&_mutex);

  // No data is directly available.
  if (!_total_events) {
    // Wait a while if subscriber was not shutdown.
    if (_process_in && _process_out) {
      if ((time_t)-1 == timeout)
        _cv.wait(&_mutex);
      else {
        time_t now(time(NULL));
        if (now < timeout) {
          bool timedout(!_cv.wait(&_mutex, (timeout - now) * 1000));
          if (timed_out)
            *timed_out = timedout;
        }
        else if (timed_out)
          *timed_out = true;
      }
      if (_total_events) {
        _get_last_event(event);
        lock.unlock();
        logging::debug(logging::low) << "multiplexing: "
          << _total_events << " events remaining in subcriber";
      }
      else
        event.clear();
    }
    // If subscriber is shutdown, notify caller.
    else
      throw (io::exceptions::shutdown(true, true)
             << "thread is shutdown, cannot get any further event");
  }
  // Data is available, no need to wait.
  else {
    if (!_process_in && _process_out)
      throw (io::exceptions::shutdown(true, false)
             << "thread is shutdown, cannot get any further event");
    _get_last_event(event);
    lock.unlock();
    logging::debug(logging::low) << "multiplexing: " << _total_events
      << " events remaining in subscriber";
  }
  return ;
}

/**
 *  Generate statistics about the subscriber.
 *
 *  @param[out] buffer Output buffer.
 */
void subscriber::statistics(std::string& buffer) const {
  QMutexLocker lock(&_mutex);
  std::ostringstream oss;
  oss << "queued_events=" << _events.size() << "\n";
  buffer.append(oss.str());
  return ;
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 */
void subscriber::write(misc::shared_ptr<io::data> const& event) {
  {
    QMutexLocker lock(&_mutex);
    if (++_total_events > event_queue_max_size()
        && !_temporary.isNull())
      _temporary->write(event);
    else
      _events.enqueue(event);
  }
  _cv.wakeOne();
  return ;
}

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
  assert(!"subscriber is not copyable");
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
  assert(!"subscriber is not copyable");
  abort();
  return (*this);
}

/**
 *  Release all events stored within the internal list.
 */
void subscriber::clean() {
  QMutexLocker lock(&_mutex);
  if (_endp_temporary.get())
    _temporary = _endp_temporary->open();
  _events.clear();
  _total_events = 0;
  return ;
}

/**
 *  Get the last event available from the internal queue. Warning, lock
 *  _mutex before use this function.
 *
 *  @param[out] event Last event available.
 */
void subscriber::_get_last_event(misc::shared_ptr<io::data>& event) {
  if (_total_events > event_queue_max_size()
      && !_temporary.isNull()) {
    _temporary->read(event);
    _events.enqueue(event);
  }
  event = _events.dequeue();
  --_total_events;
}
