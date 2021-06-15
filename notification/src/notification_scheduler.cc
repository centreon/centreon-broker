/*
** Copyright 2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/notification/notification_scheduler.hh"
#include <QMutexLocker>
#include <limits>
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/notification/state.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 *
 *  @param[in] st     The state object.
 *  @param[in] cache  The data cache object.
 */
notification_scheduler::notification_scheduler(state& st, node_cache& cache)
    : _should_exit{false},
      _general_mutex{},
      _state(st),
      _cache(cache),
      _started_flag{false} {}

/**
 *  Called by the notification thread when it starts.
 */
void notification_scheduler::run() {
  while (1) {
    // Lock the general mutex used by the notification scheduler.
    std::unique_lock<std::mutex> lock(_general_mutex);

    // Wait until the first action in the queue - or forever until awakened
    // if the queue is empty.
    time_t first_time = _queue.get_first_time();
    time_t now = ::time(NULL);
    unsigned long wait_for = first_time == time_t(-1)
                                 ? std::numeric_limits<unsigned long>::max()
                             : (first_time >= now) ? (first_time - now) * 1000
                                                   : 0;
    log_v2::notification()->debug(
        "notification: scheduler sleeping for {} seconds", wait_for / 1000.0);

    _general_condition.wait_for(lock, std::chrono::milliseconds(wait_for));

    log_v2::notification()->debug("notification: scheduler waking up");

    // The should exit flag was set - exit.
    if (_should_exit)
      break;

    // Process the actions and release the mutex.
    _process_actions();
  }
}

/**
 *  Start the notification scheduler and wait until it has started.
 */
void notification_scheduler::start() {
  _thread = std::thread(&notification_scheduler::run, this);
  _started_flag = true;
}

void notification_scheduler::wait() {
  _thread.join();
  _started_flag = false;
}

/**
 *  Ask gracefully for the notification thread to exit.
 */
void notification_scheduler::exit() throw() {
  // Set the should exit flag.
  {
    std::lock_guard<std::mutex> lock(_general_mutex);
    _should_exit = true;
    // Wake the notification scheduling thread.
    _general_condition.notify_all();
  }
}

/**
 *  @brief Add an action to the internal queue.
 *
 *  Can be called outside or inside the notif thread context.
 *
 *  @param at  The time of the action.
 *  @param a   The action.
 */
void notification_scheduler::add_action_to_queue(time_t at, action a) {
  bool need_to_wake = false;
  // Add the action to the queue.
  {
    std::lock_guard<std::mutex> lock(_general_mutex);
    // If we just replaced the first event, we need to wake the scheduling
    // thread.
    time_t first_time(_queue.get_first_time());
    if ((first_time > at) || ((time_t)-1 == first_time))
      need_to_wake = true;
    _queue.run(at, a);
    // Wake the notification scheduling thread if needed.
    if (need_to_wake)
      _general_condition.notify_all();
  }
}

/**
 *  @brief Remove all the actions associated to a node.
 *
 *  Called outside the notif thread context.
 *
 *  @param[in] id   The id of the node.
 */
void notification_scheduler::remove_actions_of_node(objects::node_id id) {
  {
    std::lock_guard<std::mutex> lock(_general_mutex);
    // Get all the action of a particular node.
    time_t first_time = _queue.get_first_time();
    std::vector<const action*> actions = _queue.get_actions_of_node(id);
    // Iterate over the actions to remove them.
    for (std::vector<const action*>::iterator it(actions.begin()),
         end(actions.end());
         it != end; ++it)
      _queue.remove(**it);
    // If we just deleted the first event, we need to wake
    // the scheduling thread.
    if (_queue.get_first_time() != first_time)
      _general_condition.notify_all();
  }
}

/**
 *  @brief Called repeatedly by the notification thread to process actions.
 *
 *  This method releases the mutex as soon as possible to prevent long
 *  mutex locking.
 */
void notification_scheduler::_process_actions() {
  // Move the global queue to a local queue and release the mutex.
  // That way, we can add new actions in an external thread while this thread
  // is processing those actions.
  run_queue local_queue;
  time_t now = ::time(NULL);
  _queue.move_to_queue(local_queue, now);
  _general_mutex.unlock();

  // Iterate on the local queue.
  for (run_queue::iterator it(local_queue.begin()), end(local_queue.end());
       it != end; ++it) {
    // The action processing can add other actions to the queue.
    std::vector<std::pair<time_t, action> > spawned_actions;
    {
      // Lock the state mutex.
      std::unique_ptr<QReadLocker> lock(_state.read_lock());
      // Process the action.
      it->second->process_action(_state, _cache, spawned_actions);
    }
    // Add the spawned action to the queue.
    _schedule_actions(spawned_actions);
  }
}

/**
 *  Schedule several actions.
 *
 *  @param[in] actions  The actions to schedule.
 */
void notification_scheduler::_schedule_actions(
    std::vector<std::pair<time_t, action> > const& actions) {
  for (std::vector<std::pair<time_t, action> >::const_iterator
           it(actions.begin()),
       end(actions.end());
       it != end; ++it)
    add_action_to_queue(it->first, it->second);
}
