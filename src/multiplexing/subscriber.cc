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
**
** For more information: contact@centreon.com
*/

#include <algorithm>
#include <assert.h>
#include <memory>
#include <stdlib.h>
#include "concurrency/lock.hh"
#include "events/event.hh"
#include "multiplexing/internal.hh"
#include "multiplexing/subscriber.hh"

using namespace multiplexing;

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
  : interface::destination(s),
    interface::source(s) {
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
  concurrency::lock l(_mutex);
  while (!_events.empty()) {
    events::event* event = _events.front();
    _events.pop();
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
  concurrency::lock l(gl_subscribersm);
  gl_subscribers.push_back(this);
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
  concurrency::lock l(gl_subscribersm);
  std::remove(gl_subscribers.begin(), gl_subscribers.end(), this);
  _cv.wake_all();
  return ;
}

/**
 *  Get the next available event.
 *
 *  @return Next available event.
 */
events::event* subscriber::event() {
  return (this->event(-1));
}

/**
 *  Get the next available event without waiting after deadline.
 *
 *  @param[in] deadline Date that shouldn't be exceeded while waiting
 *                      for a new event.
 *
 *  @return Next available event, NULL if timeout occured.
 */
events::event* subscriber::event(time_t deadline) {
  std::auto_ptr<events::event> event;
  concurrency::lock l(_mutex);
  if (_events.empty()) {
    if (-1 == deadline)
      _cv.sleep(_mutex);
    else
      _cv.sleep(_mutex, deadline);
    if (!_events.empty()) {
      event.reset(_events.front());
      _events.pop();
    }
  }
  else {
    event.reset(_events.front());
    _events.pop();
  }
  return (event.release());
}

/**
 *  Add a new event to the internal event list.
 *
 *  @param[in] event Event to add.
 */
void subscriber::event(events::event* event) {
  concurrency::lock l(_mutex);
  _events.push(event);
  _cv.wake();
  return ;
}
