/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_NOTIFICATION_SCHEDULER_HH
#  define CCB_NOTIFICATION_NOTIFICATION_SCHEDULER_HH

#  include <ctime>
#  include <map>
#  include <QThread>
#  include <QMutex>
#  include <QWaitCondition>
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
  class        notification_scheduler : public QThread {
  public:
               notification_scheduler(
                  ::com::centreon::broker::notification::state& st,
                  ::com::centreon::broker::notification::node_cache& cache);

    void       exit() throw ();
    void       add_action_to_queue(time_t at, action a);
    void       remove_actions_of_node(objects::node_id id);

  protected:
    void       run();

  private:
    run_queue  _queue;
    bool       _should_exit;
    QMutex     _general_mutex;
    QWaitCondition
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
