/*
** Copyright 2009-2014 Merethis
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

#include <limits>
#include <QMutexLocker>
#include "com/centreon/broker/notification/notification_scheduler.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
notification_scheduler::notification_scheduler()
  : _should_exit(false),
    _general_mutex(QMutex::Recursive) {}

/**
 *  Called by the notification thread when it starts.
 */
void notification_scheduler::run() {
  _general_mutex.lock();
  while (1) {
    // Wait until the first action in the queue - or forever until awakened
    // if the queue is empty.
    time_t first_time = _queue.get_first_time();
    time_t now = time(NULL);
    unsigned long wait_for = first_time == time_t(-1) ?
                               std::numeric_limits<unsigned long>::max()
                               : (first_time >= now) ?
                                   (first_time - now) * 1000
                                   : 0;

    _general_condition.wait(&_general_mutex, wait_for);

    // The should exit flag was set - exit.
    if (_should_exit)
      break;

    // Process the actions.
    _process_actions();
  }
}

/**
 *  Ask gracefully for the notification thread to exit.
 */
void notification_scheduler::exit() throw () {
  // Set the should exit flag.
  {
    QMutexLocker lock(&_general_mutex);
    _should_exit = true;
  }
  // Wake the notification scheduling thread.
  _general_condition.wakeAll();
}

/**
 *  @brief Add an action to the internal queue.
 *
 *  This is perfectly thread safe in and out of the notification thread.
 *
 *  @param at  The time of the action.
 *  @param a   The action.
 */
void notification_scheduler::add_action_to_queue(time_t at, action a) {
  bool need_to_wake = false;
  // Add the action to the queue.
  {
    QMutexLocker lock(&_general_mutex);
    // If we just replaced the first event, we need to wake the scheduling
    // thread.
    if (_queue.get_first_time() > at)
      need_to_wake = true;
    _queue.run(at, a);
  }
  // Wake the notification scheduling thread if needed.
  if (need_to_wake)
    _general_condition.wakeAll();
}

/**
 *  Called repeatedly by the notification thread to process actions.
 */
void notification_scheduler::_process_actions() {
  time_t now = time(NULL);
  for (run_queue::iterator it(_queue.begin()), end(_queue.end());
       it != end;) {
    if (it->first > now)
      return;

    // Processing

      ++it;
      _queue.remove_first();
  }
}
