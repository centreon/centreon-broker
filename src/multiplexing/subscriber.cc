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
subscriber::subscriber(subscriber const& s)
  : io::istream(), serialization::iserial(), io::ostream(), serialization::oserial(), serialization::serial() {
  (void)s;
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
  while (!_events.isEmpty()) {
    QSharedPointer<events::event> event(_events.dequeue());
    event->remove_reader();
  }
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
QSharedPointer<events::event> subscriber::deserialize() {
  return (this->deserialize(-1));
}

/**
 *  Get the next available event without waiting after deadline.
 *
 *  @param[in] deadline Date that shouldn't be exceeded while waiting
 *                      for a new event.
 *
 *  @return Next available event, NULL if timeout occured.
 */
QSharedPointer<events::event> subscriber::deserialize(time_t deadline) {
  QSharedPointer<events::event> event;
  QMutexLocker lock(&_mutex);
  if (_registered) {
    if (_events.empty()) {
      if (-1 == deadline)
        _cv.wait(&_mutex);
      else
        _cv.wait(&_mutex, deadline);
      if (!_events.empty()) {
        event = QSharedPointer<events::event>(_events.dequeue());
      }
    }
    else
      event = QSharedPointer<events::event>(_events.dequeue());
  }
  return (event);
}

/**
 *  Try to read from the subscriber.
 *
 *  @param[in] data Unused.
 *  @param[in] size Unused.
 *
 *  @return Does not return, throw an exception.
 */
unsigned int subscriber::read(void* data, unsigned int size) {
  (void)data;
  (void)size;
  throw (exceptions::basic() << "attempt to read from a subscriber");
  return (0);
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 */
void subscriber::serialize(QSharedPointer<events::event> event) {
  QMutexLocker lock(&_mutex);
  _events.enqueue(event);
  _cv.wakeOne();
  return ;
}

/**
 *  Try to write to the subscriber.
 *
 *  @param[in] data Unused.
 *  @param[in] size Unused.
 *
 *  @return Does not return, throw an exception.
 */
unsigned int subscriber::write(void const* data, unsigned int size) {
  (void)data;
  (void)size;
  throw (exceptions::basic() << "attempt to write to a subscriber");
  return (0);
}
