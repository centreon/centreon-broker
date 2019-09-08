/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_NOTIFICATION_DEPENDENCY_HH
#define CCB_NOTIFICATION_DEPENDENCY_HH

#include <functional>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace notification {
namespace objects {
/**
 *  @class dependency dependency.hh
 * "com/centreon/broker/notification/objects/dependency.hh"
 *  @brief Dependency object.
 *
 *  The object containing a dependency.
 */
class dependency {
 public:
  DECLARE_SHARED_PTR(dependency);

  enum type { unknown = 0, host, service };

  enum action_on {
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

  enum dependency_kind {
    unknown_dependency = 0,
    notification_dependency,
    execution_dependency
  };

  dependency();
  dependency(dependency const& obj);
  dependency& operator=(dependency const& obj);

  bool is_host_dependency() const throw();
  void set_is_host_dependency(bool val) throw();
  bool is_service_dependency() const throw();
  void set_is_service_dependency(bool val) throw();

  std::string const& get_period() const throw();
  void set_period(std::string const& val);
  dependency_kind get_kind() const throw();
  void set_kind(dependency_kind val) throw();
  action_on get_execution_failure_options() const throw();
  void set_execution_failure_options(action_on val) throw();
  void set_execution_failure_option(action_on val) throw();
  bool is_execution_failure_option_set(action_on val) throw();
  bool get_inherits_parent() const throw();
  void set_inherits_parent(bool val) throw();
  action_on get_notification_failure_options() const throw();
  void set_notification_failure_options(action_on val) throw();
  void set_notification_failure_option(action_on val) throw();
  bool is_notification_failure_option_set(action_on val) const throw();

  void parse_notification_failure_options(std::string const& line);
  void parse_execution_failure_options(std::string const& line);

 private:
  type _type;
  std::string _dependency_period;
  dependency_kind _dependency_type;
  action_on _execution_failure_options;
  bool _inherits_parent;
  action_on _notification_failure_options;

  struct name_to_action {
    const char* name;
    action_on action;
  };

  static const name_to_action _service_actions[];
  static const name_to_action _host_actions[];

  void _parse_failure_options(std::string const& line,
                              void (dependency::*func_to_call)(action_on));
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_DEPENDENCY_HH
