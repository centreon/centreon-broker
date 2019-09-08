/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_RUN_QUEUE_HH
#define CCB_NOTIFICATION_RUN_QUEUE_HH

#include <ctime>
#include <map>
#include <set>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/action.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class run_queue run_queue.hh
 * "com/centreon/broker/notification/run_queue.hh"
 *  @brief A run queue of actions.
 *
 *  This class is optimized to search an action by time and by node.
 */
class run_queue {
 public:
  typedef std::multimap<time_t, const action*> action_map;
  typedef action_map::iterator iterator;
  typedef action_map::const_iterator const_iterator;

  run_queue();
  run_queue(run_queue const& obj);
  run_queue& operator=(run_queue const& obj);
  void move_to_queue(run_queue& obj, time_t until);

  void run(time_t at, action a);
  void remove_first();
  void remove(action const& a);

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  std::vector<const action*> get_actions_of_node(objects::node_id id);

  time_t get_first_time() const throw();

 private:
  action_map _action_by_time;
  std::multimap<objects::node_id, const action*> _action_by_node;
  std::set<action> _action_set;

  void _rebuild_set();
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_RUN_QUEUE_HH
