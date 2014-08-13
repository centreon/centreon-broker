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
#  include "com/centreon/broker/notification/objects/group.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       notification {

  class         dependency {
  public:
    enum        type {
      unknown = 0,
      host,
      service
    };

    enum        action_on {
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

    enum        dependency_kind {
      unknown_dependency = 0,
      notification_dependency,
      execution_dependency
    };

    dependency();
    dependency(dependency const& obj);
    dependency& operator=(dependency const& obj);

    bool                   is_host_dependency() const throw();
    void                   set_is_host_dependency(bool val) throw();
    bool                   is_service_dependency() const throw();
    void                   set_is_service_dependency(bool val) throw();

    std::string const&    get_period() const throw();
    void                  set_period(std::string const& val);
    dependency_kind       get_kind() const throw();
    void                  set_kind(dependency_kind val) throw();
    group const&          get_dependent_hostgroups() const throw();
    void                  set_dependent_hostgroups(group const& val);
    group const&          get_dependent_hosts() const throw();
    void                  set_dependent_hosts(group const& val);
    group const&          get_dependent_servicegroups() const throw();
    void                  set_dependent_servicegroups(group const& val);
    group const&          get_dependent_service_description() const throw();
    void                  set_dependent_service_description(group const& val);
    unsigned int          get_execution_failure_options() const throw();
    void                  set_execution_failure_options(unsigned int val) throw();
    group const&          get_hostgroups() const throw();
    void                  set_hostgroups(group const& val);
    group const&          get_hosts() const throw();
    void                  set_hosts(group const& val);
    bool                  get_inherits_parent() const throw();
    void                  set_inherits_parent(bool val) throw();
    unsigned int          get_notification_failure_options() const throw();
    void                  set_notification_failure_options(unsigned int val) throw();
    group const&          get_servicegroups() const throw();
    void                  set_servicegroups(group const& val);
    group const&          get_service_description() const throw();
    void                  set_service_description(group const& val);

  private:
    type                   _type;
    std::string            _dependency_period;
    dependency_kind        _dependency_type;
    group                  _dependent_hostgroups;
    group                  _dependent_hosts;
    group                  _dependent_servicegroups;
    group                  _dependent_service_description;
    unsigned int           _execution_failure_options;
    group                  _hostgroups;
    group                  _hosts;
    bool                   _inherits_parent;
    unsigned int           _notification_failure_options;
    group                  _servicegroups;
    group                  _service_description;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_DEPENDENCY_HH
