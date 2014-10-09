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

#ifndef CCB_NOTIFICATION_ACTION_HH
#  define CCB_NOTIFICATION_ACTION_HH

#  include <vector>
#  include <utility>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"

CCB_BEGIN()

namespace           notification {
  // Forward declaration.
  class state;
  /**
   *  @class action action.hh "com/centreon/broker/notification/action.hh"
   *  @brief Action object.
   *
   *  Represent an action requested to the notification engine.
   */
  class               action {
  public:
    /**
     *  The type of the action.
     */
    enum              action_type {
      unknown = 0,
      notification_processing,
      notification_attempt
    };
                      action();
                      action(action const& obj);
    action&           operator=(action const& obj);

    action_type       get_type() const throw();
    void              set_type(action_type type) throw();

    objects::node_id  get_node_id() const throw();
    void              set_node_id(objects::node_id id) throw();

    unsigned int      get_notification_rule_id() const throw();
    void              set_notification_rule_id(unsigned int id) throw();

    void            process_action(
                        state& st,
                        std::vector<std::pair<time_t, action> >& spawned_actions);
  private:
    action_type       _act;
    objects::node_id  _id;
    unsigned int      _notification_rule_id;

    enum              return_value {
      ok = 0,
      error_should_reschedule,
      error_should_remove
    };

    void              _spawn_notification_attempts(
                        ::com::centreon::broker::notification::state& st,
                        std::vector<std::pair<time_t, action> >& spawned_actions);
    bool              _check_action_viability(
                        ::com::centreon::broker::notification::state& st);


    void              _process_notification(
                       ::com::centreon::broker::notification::state& st,
                        std::vector<std::pair<time_t, action> >& spawned_actions);
    return_value      _check_notification_node_viability(
                       ::com::centreon::broker::notification::state& st);
    return_value      _check_notification_contact_viability(
                       objects::contact::ptr con,
                       ::com::centreon::broker::notification::state& st);
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_ACTION_HH
