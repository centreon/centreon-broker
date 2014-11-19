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

#ifndef CCB_NOTIFICATION_RUN_QUEUE_HH
#  define CCB_NOTIFICATION_RUN_QUEUE_HH

#  include <ctime>
#  include <map>
#  include <set>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/action.hh"

CCB_BEGIN()

namespace           notification {
  /**
   *  @class run_queue run_queue.hh "com/centreon/broker/notification/run_queue.hh"
   *  @brief A run queue of actions.
   *
   *  This class is optimized to search an action by time and by node.
   */
  class               run_queue {
  public:
    typedef           std::multimap<time_t, const action*> action_map;
    typedef           action_map::iterator iterator;
    typedef           action_map::const_iterator const_iterator;

                      run_queue();
                      run_queue(run_queue const& obj);
    run_queue&        operator=(run_queue const& obj);
    void              move_to_queue(run_queue& obj, time_t until);

    void              run(time_t at, action a);
    void              remove_first();
    void              remove(action const& a);

    iterator          begin();
    const_iterator    begin() const;
    iterator          end();
    const_iterator    end() const;

    std::vector<const action*>
                      get_actions_of_node(objects::node_id id);

    time_t            get_first_time() const throw();

  private:
    action_map        _action_by_time;
    std::multimap<objects::node_id, const action*>
                      _action_by_node;
    std::set<action>  _action_set;

    void              _rebuild_set();
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_RUN_QUEUE_HH
