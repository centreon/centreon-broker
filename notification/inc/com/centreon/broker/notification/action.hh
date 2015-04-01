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
#  include "com/centreon/broker/notification/node_cache.hh"

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
      notification_processing,  // This notification is yet to be processed.
      notification_attempt,     // Normal notification.
      notification_up,          // Up notification.
      notification_ack,         // Ack notification.
      notification_downtime     // Downtime notification.
    };
                      action();
                      action(action const& obj);
    action&           operator=(action const& obj);
    bool              operator==(action const& obj) const;
    bool              operator<(action const& obj) const;

    action_type       get_type() const throw();
    void              set_type(action_type type) throw();

    action_type       get_forwarded_type() const throw();
    void              set_forwarded_type(action_type type) throw();

    objects::node_id  get_node_id() const throw();
    void              set_node_id(objects::node_id id) throw();

    unsigned int      get_notification_rule_id() const throw();
    void              set_notification_rule_id(unsigned int id) throw();

    unsigned int      get_notification_number() const throw();
    void              set_notification_number(unsigned int num) throw();

    time_t            get_at() const throw();
    void              set_at(time_t at) throw();

    void              process_action(
                        state& st,
                        node_cache& cache,
                        std::vector<std::pair<time_t, action> >&
                          spawned_actions) const;
  private:
    action_type       _act;
    action_type       _forwarded_action;
    objects::node_id  _id;
    unsigned int      _notification_rule_id;
    unsigned int      _notification_number;
    time_t            _at;

    void              _spawn_notification_attempts(
                        ::com::centreon::broker::notification::state& st,
                        std::vector<std::pair<time_t, action> >&
                          spawned_actions) const;
    bool              _check_action_viability(
                        ::com::centreon::broker::notification::state& st) const;
    void              _process_notification(
                       ::com::centreon::broker::notification::state& st,
                       node_cache& cache,
                        std::vector<std::pair<time_t, action> >&
                          spawned_actions) const;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_ACTION_HH
