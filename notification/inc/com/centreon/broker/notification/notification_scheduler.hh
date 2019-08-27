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

#ifndef CCB_NOTIFICATION_NOTIFICATION_SCHEDULER_HH
#  define CCB_NOTIFICATION_NOTIFICATION_SCHEDULER_HH

#  include <condition_variable>
#  include <ctime>
#  include <map>
#  include <mutex>
#  include <thread>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/action.hh"
#  include "com/centreon/broker/notification/run_queue.hh"

CCB_BEGIN()

namespace             notification {
  // Forward declaration.
  class state;
  class node_cache;

  /**
   *  @class notification_scheduler notification_scheduler.hh "com/centreon/broker/notification/notification_scheduler.hh"
   *  @brief The notification scheduler.
   *
   *  Manage a thread as a notification scheduler.
   */
  class        notification_scheduler {
  public:
               notification_scheduler(
                  ::com::centreon::broker::notification::state& st,
                  ::com::centreon::broker::notification::node_cache& cache);

    void       start();
    void       exit() throw ();
    void       add_action_to_queue(time_t at, action a);
    void       remove_actions_of_node(objects::node_id id);
    void wait();

  protected:
    void       run();

  private:
    std::thread _thread;

    // only used by master
    bool _started_flag;

    run_queue  _queue;
    bool       _should_exit;
    std::mutex     _general_mutex;
    std::condition_variable
               _general_condition;

    void       _process_actions();
    void       _schedule_actions(
                 std::vector<std::pair<time_t, action> > const& actions);

                notification_scheduler(notification_scheduler const& obj);
    notification_scheduler&
                operator=(notification_scheduler const& obj);

    ::com::centreon::broker::notification::state&
                _state;
    ::com::centreon::broker::notification::node_cache&
                _cache;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_NOTIFICATION_SCHEDULER_HH
