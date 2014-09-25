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
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/action.hh"

CCB_BEGIN()

namespace           notification {
  /**
   *  @class run_queue run_queue.hh "com/centreon/broker/notification/run_queue.hh"
   *  @brief A run queue of actions.
   *
   *  Store actions tried by start time.
   */
  class               run_queue {
  public:
    typedef           std::multimap<time_t, action> action_map;
    typedef           action_map::iterator iterator;
    typedef           action_map::const_iterator const_iterator;

                      run_queue();
                      run_queue(run_queue const& obj);
    run_queue&        operator=(run_queue const& obj);

    void              run(time_t at, action a);

    iterator          begin();
    const_iterator    begin() const;
    iterator          end();
    const_iterator    end() const;

    time_t            get_first_time() const throw();

  private:
    action_map        _actions;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_RUN_QUEUE_HH
