/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_NODE_HH
#  define CCB_NOTIFICATION_NODE_HH

#  include <ctime>
#  include <set>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/defines.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/timeperiod.hh"

CCB_BEGIN()

namespace        notification {
  namespace      objects {
    /**
     *  @class node node.hh "com/centreon/broker/notification/objects/node.hh"
     *  @brief A node object.
     *
     *  The object containing a node of the notification module.
     */
    class     node {
    public:
              DECLARE_SHARED_PTR(node);

              node();
              node(node const& obj);
              node& operator=(node const& obj);

      enum    notification_option_type {
        none = 0,
        service_unknown = (1 << 1),
        service_warning = (1 << 2),
        service_critical = (1 << 3),
        service_pending = (1 << 4),
        service_recovery = (1 << 5),
        host_down = (1 << 6),
        host_unreachable = (1 << 7),
        host_recovery = (1 << 8)
      };

      int     get_notification_number() const throw();
      void    set_notification_number(int value);
      time_t  get_last_notification_time() const throw();
      void    set_last_notification_time(time_t value);
      node_state
              get_hard_state() const throw();
      void    set_hard_state(node_state value);
      node_state
              get_soft_state() const throw();
      void    set_soft_state(node_state value);
      node_id get_node_id() const throw();
      void    set_node_id(node_id) throw();
      std::set<node_id> const&
              get_parents() const throw();
      void    add_parent(node_id id);
      void    remove_parent(node_id id);
      bool    has_parent() const throw();
      bool    get_notifications_enabled() const throw();
      void    set_notifications_enabled(bool enable) throw();
      std::string const&
              get_notification_timeperiod() const throw();
      void    set_notification_timeperiod(std::string const& tp);
      double  get_notification_interval() const throw();
      void    set_notification_interval(double val) throw();
      node_notification_opt
              get_notification_options() const throw();
      void    set_notification_options(node_notification_opt val) throw();
      void    should_be_notified() const throw();

    private:
      int     _notification_number;
      bool    _notifications_enabled;
      double  _notification_interval;
      time_t  _last_notification_time;
      node_state
              _hard_state;
      node_state
              _soft_state;
      node_id _id;
      std::set<node_id>
              _parents;
      std::string
              _notification_period;
      node_notification_opt
              _notification_options;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODE_HH
