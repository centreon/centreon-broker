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

#ifndef CCB_NOTIFICATION_COMMAND_HH
#define CCB_NOTIFICATION_COMMAND_HH

#include <QRegExp>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/node.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/objects/notification_method.hh"
#include "com/centreon/broker/notification/objects/notification_rule.hh"
#include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace notification {
// Forward declaration.
class action;
class state;

namespace objects {
/**
 *  @class command command.hh
 * "com/centreon/broker/notification/objects/command.hh"
 *  @brief Command object.
 *
 *  The object containing a command.
 */
class command {
 public:
  DECLARE_SHARED_PTR(command);

  command(std::string const& base_command);
  command(command const& obj);
  command& operator=(command const& obj);

  bool get_enable_shell() const throw();
  void set_enable_shell(bool val);
  std::string const& get_name() const throw();
  void set_name(std::string const& name);

  std::string resolve(contact::ptr const& contact,
                      node::ptr const& n,
                      node_cache const& cache,
                      state const& st,
                      action const& act);

 private:
  command();

  bool _enable_shell;
  std::string _name;
  std::string _base_command;

  static const QRegExp _macro_regex;

  // static              _macro_init();
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_COMMAND_HH
