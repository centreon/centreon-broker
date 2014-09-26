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

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace           notification {
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
      notification_attempt
    };
                      action();
                      action(action const& obj);
    action&           operator=(action const& obj);

    action_type       get_type() const throw();
    void              set_type(action_type type) throw();

    objects::node_id  get_node_id() const throw();
    void              set_node_id(objects::node_id id) throw();

  private:
    action_type       _act;
    objects::node_id  _id;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_ACTION_HH
