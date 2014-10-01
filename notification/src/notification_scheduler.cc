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
#include "com/centreon/broker/notification/state.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
notification_scheduler::notification_scheduler(state& st)
  : _should_exit(false),
    _general_mutex(QMutex::Recursive),
    _state(st) {}

/**
 *  Called by the notification thread when it starts.
 */
void notification_scheduler::run() {
  while (1) {
    // Lock the general mutex used by the notification scheduler.
    _general_mutex.lock();
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

    // Process the actions and release the mutex.
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
  // Move the global queue to a local queue and release the global mutex.
  // That way, we can add new actions in an external thread while this thread
  // processing those actions.
  run_queue local_queue;
  _queue.swap(local_queue);
  _general_mutex.unlock();

  // Iterate on the local queue.
  time_t now = time(NULL);
  for (run_queue::iterator it(local_queue.begin()), end(local_queue.end());
       it != end;) {
    if (it->first > now)
      return;

    // Get the viability of this action.
    action::viability action_viability;
    {
      // Lock the state mutex.
      std::auto_ptr<QMutexLocker> lock(_state.lock());
      // Process the action.
      action_viability = it->second.check_action_viability(_state);
    }
    // A rescheduling was asked.
    if (action_viability == action::reschedule)
      _reschedule_action(it->first, it->second);
    // The action is ok: process it.
    else if (action_viability == action::ok);

    ++it;
  }
}

void notification_scheduler::_reschedule_action(time_t previously_scheduled,
                                                action a) {
  // Reschedule at the previous scheduling + the notification interval of the node.
  double notification_interval = 0;
  {
    std::auto_ptr<QMutexLocker> lock(_state.lock());
    node::ptr n = _state.get_node_by_id(a.get_node_id());
    // The node doesn't exist anymore. Silently eat the action.
    if (!n)
      return;
    notification_interval = n->get_notification_interval();
  }
  add_action_to_queue(previously_scheduled + notification_interval, a);
}
