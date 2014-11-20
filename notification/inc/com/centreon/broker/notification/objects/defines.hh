/*
** Copyright 2014 Merethis
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

#ifndef CCB_NOTIFICATION_OBJECTS_DEFINES_HH
#  define CCB_NOTIFICATION_OBJECTS_DEFINES_HH

#  include "com/centreon/broker/namespace.hh"


CCB_BEGIN()

namespace               notification {
  namespace             objects {
    class               node_state {
    public:
      enum              state_type {
        ok = 0,
        host_up = 0,
        host_down = 1,
        host_unreachable = 2,
        service_ok = 0,
        service_warning = 1,
        service_critical = 2,
        service_unknown = 3
      };

                        node_state();
                        node_state(state_type type);
                        node_state(short);
                        node_state(int);
                        node_state(node_state const& st);
      node_state&       operator=(node_state const& st);
      bool              operator==(node_state const& st) const throw();
      bool              operator==(node_state::state_type type) const throw();

      operator          short() const throw();
      operator          int() const throw();

      state_type        value;
    };

    class               node_notification_opt {
    public:
      enum              notification_type {
        none = 0,
        service_unknown = (1 << 1),
        service_warning = (1 << 2),
        service_critical = (1 << 3),
        service_pending = (1 << 4),
        service_recovery = (1 << 5),
        host_down = (1 << 6),
        host_unreachable = (1 << 7),
        host_recovery = (1 << 8),
        not_correlated = (1 << 9)
      };

                        node_notification_opt();
                        node_notification_opt(notification_type type);
                        node_notification_opt(int);
                        node_notification_opt(node_notification_opt const&);
      node_notification_opt&
                        operator=(node_notification_opt const&);
      bool              operator==(
                          node_notification_opt const& opt) const throw();
      bool              operator==(
                          node_notification_opt::notification_type type) const throw();


      operator          int() const throw();

      bool              check_for(notification_type opt) const throw();
      void              add_option(notification_type opt) throw();

      notification_type value;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_OBJECTS_DEFINES_HH
