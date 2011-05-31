/*
** Copyright 2009-2011 MERETHIS
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

#include <algorithm>
#include <assert.h>
#include <QMutexLocker>
#include <QScopedPointer>
#include <stdlib.h>
#include "events/event.hh"
#include "exceptions/basic.hh"
#include "multiplexing/internal.hh"
#include "multiplexing/subscriber.hh"

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
  QMutexLocker lock(&gl_subscribersm);
  gl_subscribers.push_back(this);
  _registered = true;
}

/**
 *  Destructor.
 */
subscriber::~subscriber() {
  clean();
  this->close();
}

/**
 *  Unregister from event publishing notifications.
 */
void subscriber::close() {
  QMutexLocker lock(&gl_subscribersm);
  std::remove(gl_subscribers.begin(), gl_subscribers.end(), this);
  _registered = false;
  _cv.wakeAll();
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
  if (_registered) {
    if (_events.empty()) {
      if (-1 == deadline)
        _cv.wait(&_mutex);
      else
        _cv.wait(&_mutex, deadline);
      if (!_events.empty()) {
        event = _events.dequeue();
      }
    }
    else
      event = _events.dequeue();
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
