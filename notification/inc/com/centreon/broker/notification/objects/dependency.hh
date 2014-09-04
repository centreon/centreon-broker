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

#ifndef CCB_NOTIFICATION_DEPENDENCY_HH
#  define CCB_NOTIFICATION_DEPENDENCY_HH

#  include <string>
#  include <functional>
#  include "com/centreon/broker/notification/objects/group.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace                 notification {
  /**
   *  @class dependency dependency.hh "com/centreon/broker/notification/objects/dependency.hh"
   *  @brief Dependency object.
   *
   *  The object containing a dependency.
   */
  class                   dependency {
  public:
                          DECLARE_SHARED_PTR(dependency);

    enum                  type {
      unknown = 0,
      host,
      service
    };

    enum                  action_on {
      none = 0,
      host_up = (1 << 0),
      host_down = (1 << 1),
      host_unreachable = (1 << 2),
      host_pending = (1 << 3),
      service_ok = (1 << 4),
      service_unknown = (1 << 5),
      service_warning = (1 << 6),
      service_critical = (1 << 7),
      service_pending = (1 << 8)
    };

    enum                  dependency_kind {
      unknown_dependency = 0,
      notification_dependency,
      execution_dependency
    };

                          dependency();
                          dependency(dependency const& obj);
                          dependency& operator=(dependency const& obj);

    bool                  is_host_dependency() const throw();
    void                  set_is_host_dependency(bool val) throw();
    bool                  is_service_dependency() const throw();
    void                  set_is_service_dependency(bool val) throw();

    std::string const&    get_period() const throw();
    void                  set_period(std::string const& val);
    dependency_kind       get_kind() const throw();
    void                  set_kind(dependency_kind val) throw();
    action_on             get_execution_failure_options() const throw();
    void                  set_execution_failure_options(action_on val) throw();
    void                  set_execution_failure_option(action_on val) throw();
    bool                  is_execution_failure_option_set(
                            action_on val) throw();
    bool                  get_inherits_parent() const throw();
    void                  set_inherits_parent(bool val) throw();
    action_on             get_notification_failure_options() const throw();
    void                  set_notification_failure_options(
                            action_on val) throw();
    void                  set_notification_failure_option(
                            action_on val) throw();
    bool                  is_notification_failure_option_set(
                            action_on val) const throw();

    void                  parse_notification_failure_options(
                            std::string const& line);
    void                  parse_execution_failure_options(
                            std::string const& line);

  private:
    type                  _type;
    std::string           _dependency_period;
    dependency_kind       _dependency_type;
    action_on             _execution_failure_options;
    bool                  _inherits_parent;
    action_on             _notification_failure_options;

    struct                name_to_action {
      const char* name;
      action_on action;
    };

    static const name_to_action
                          _service_actions[];
    static const name_to_action
                          _host_actions[];

    void _parse_failure_options(
            std::string const& line,
            void (dependency::*func_to_call)(action_on));
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_DEPENDENCY_HH
