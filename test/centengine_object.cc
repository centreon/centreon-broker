/*
** Copyright 2015 Centreon
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

#include "test/centengine_object.hh"
#include "test/vars.hh"

using namespace com::centreon::broker::test;

/**
 *  Constructor.
 *
 *  @param[in] type  Object type.
 */
centengine_object::centengine_object(centengine_object::object_type type)
    : _type(type) {
  switch (_type) {
    case command_type:
      set("command_line", MY_PLUGIN_PATH " 0");
      break;
    case contact_type:
      set("host_notification_period", "default_timeperiod");
      set("host_notification_commands", "default_command");
      set("service_notification_period", "default_timeperiod");
      set("service_notification_commands", "default_command");
      break;
    case host_type:
      set("address", "localhost");
      set("active_checks_enabled", "0");
      set("check_command", "default_command");
      set("max_check_attempts", "3");
      set("check_interval", "5");
      set("retry_interval", "3");
      set("check_period", "default_timeperiod");
      set("event_handler_enabled", "0");
      set("passive_checks_enabled", "0");
      set("notification_interval", "10");
      set("notification_period", "default_timeperiod");
      set("contacts", "default_contact");
      break;
    case service_type:
      set("active_checks_enabled", "0");
      set("check_command", "default_command");
      set("max_check_attempts", "3");
      set("check_interval", "5");
      set("retry_interval", "3");
      set("check_period", "default_timeperiod");
      set("event_handler_enabled", "0");
      set("passive_checks_enabled", "0");
      set("notification_interval", "10");
      set("notification_period", "default_timeperiod");
      set("contacts", "default_contact");
      break;
    default:;  // No default for other objects.
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
centengine_object::centengine_object(centengine_object const& other)
    : _type(other._type), _variables(other._variables) {}

/**
 *  Destructor.
 */
centengine_object::~centengine_object() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
centengine_object& centengine_object::operator=(
    centengine_object const& other) {
  if (this != &other) {
    _type = other._type;
    _variables = other._variables;
  }
  return (*this);
}

/**
 *  Get a variable.
 *
 *  @param[in] variable  Variable name.
 *
 *  @return Variable value. Empty if not set.
 */
std::string centengine_object::get(std::string const& variable) const {
  std::map<std::string, std::string>::const_iterator it(
      _variables.find(variable));
  return ((it == _variables.end()) ? "" : it->second);
}

/**
 *  Get object variables.
 *
 *  @return Object variables.
 */
std::map<std::string, std::string> const& centengine_object::get_variables()
    const {
  return (_variables);
}

/**
 *  Get object type.
 *
 *  @return Object type.
 */
centengine_object::object_type centengine_object::get_type() const {
  return (_type);
}

/**
 *  Set a variable.
 *
 *  @param[in] variable  Variable name.
 *  @param[in] value     Variable value.
 */
void centengine_object::set(std::string const& variable,
                            std::string const& value) {
  if (value.empty())
    _variables.erase(variable);
  else
    _variables[variable] = value;
  return;
}
