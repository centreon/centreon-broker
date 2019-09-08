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

#include "test/centengine_config.hh"
#include <cstring>
#include <sstream>
#include "com/centreon/engine/objects.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**
 *  Default constructor.
 */
centengine_config::centengine_config() {
  // Default directives.
  set_directive("log_file", "monitoring_engine.log");
  set_directive("command_file", "monitoring_engine.cmd");
  set_directive("command_check_interval", "-1");
  set_directive("state_retention_file", "");
  set_directive("retain_state_information", "0");
  set_directive("use_retained_program_state", "0");
  set_directive("use_retained_scheduling_info", "0");
  set_directive("check_result_reaper_frequency", "1");
  // set_directive("debug_file", "monitoring_debug.log");
  // set_directive("debug_level", "-1");
  // set_directive("debug_verbosity", "2");
  // set_directive("max_debug_file_size", "1000000000");
  // Deprecated in Centreon Engine 2.x.
  set_directive("accept_passive_host_checks", "1");
  set_directive("accept_passive_service_checks", "1");
  set_directive("event_broker_options", "-1");
  set_directive("execute_host_checks", "1");
  set_directive("execute_service_checks", "1");
  set_directive("interval_length", MONITORING_ENGINE_INTERVAL_LENGTH_STR);
  set_directive("max_service_check_spread", "1");
  set_directive("max_concurrent_checks", "200");
  set_directive("passive_host_checks_are_soft", "1");
  set_directive("service_inter_check_delay_method", "s");
  set_directive("sleep_time", "0.01");
  set_directive("status_file", "monitoring_engine_status.dat");
  set_directive("service_inter_check_delay_method", "n");
  set_directive("host_inter_check_delay_method", "n");

  // Default objects.
  {
    centengine_object obj(centengine_object::host_type);
    obj.set("host_name", "default_host");
    obj.set("alias", "default_host");
    _hosts.push_back(obj);
  }
  {
    centengine_object obj(centengine_object::service_type);
    obj.set("service_description", "default_service");
    obj.set("host_name", "default_host");
    obj.set("check_command", "default_command");
    _services.push_back(obj);
  }
  {
    centengine_object obj(centengine_object::command_type);
    obj.set("command_name", "default_command");
    obj.set("command_line", MY_PLUGIN_PATH " 0");
    _commands.push_back(obj);
  }
  {
    centengine_object obj(centengine_object::contact_type);
    obj.set("contact_name", "default_contact");
    _contacts.push_back(obj);
  }
  {
    centengine_object obj(centengine_object::timeperiod_type);
    obj.set("timeperiod_name", "default_timeperiod");
    obj.set("alias", "default_timeperiod");
    obj.set("monday", "00:00-24:00");
    obj.set("tuesday", "00:00-24:00");
    obj.set("wednesday", "00:00-24:00");
    obj.set("thursday", "00:00-24:00");
    obj.set("friday", "00:00-24:00");
    obj.set("saturday", "00:00-24:00");
    obj.set("sunday", "00:00-24:00");
    _timeperiods.push_back(obj);
  }
}

/**
 *  Destructor.
 */
centengine_config::~centengine_config() {}

/**
 *  Find a host in the host list.
 *
 *  @param[in] host_name  Host name.
 *
 *  @return Iterator to the matching entry if found,
 *          get_hosts().end() otherwise.
 */
centengine_config::objlist::iterator centengine_config::find_host(
    std::string const& host_name) {
  for (objlist::iterator it(_hosts.begin()), end(_hosts.end()); it != end; ++it)
    if (it->get("host_name") == host_name)
      return (it);
  return (_hosts.end());
}

/**
 *  Find a service in the service list.
 *
 *  @param[in] host_name            Host name.
 *  @param[in] service_description  Service description.
 *
 *  @return Iterator to the matching entry if found,
 *          get_services().end() otherwise.
 */
centengine_config::objlist::iterator centengine_config::find_service(
    std::string const& host_name,
    std::string const& service_description) {
  for (objlist::iterator it(_services.begin()), end(_services.end()); it != end;
       ++it)
    if ((it->get("host_name") == host_name) &&
        (it->get("service_description") == service_description))
      return (it);
  return (_services.end());
}

/**
 *  Set a dependency between two hosts.
 *
 *  @param[in] dependent_host  Name of dependent host.
 *  @param[in] depended_host   Name of depended host.
 */
void centengine_config::host_depends_on(std::string const& dependent_host,
                                        std::string const& depended_host) {
  centengine_object obj(centengine_object::hostdependency_type);
  obj.set("dependent_host_name", dependent_host);
  obj.set("host_name", depended_host);
  obj.set("dependency_period", "default_timeperiod");
  obj.set("inherits_parent", "0");
  obj.set("notification_failure_options", "a");
  obj.set("execution_failure_options", "a");
  _host_deps.push_back(obj);
  return;
}

/**
 *  Set a parenting relationship between two hosts.
 *
 *  @param[in] parent_host  Parent host.
 *  @param[in] child_host   Child host.
 */
void centengine_config::host_parent_of(std::string const& parent_host,
                                       std::string const& child_host) {
  objlist::iterator it(find_host(child_host));
  if (it != _hosts.end()) {
    std::string parents(it->get("parents"));
    if (parents.empty())
      it->set("parents", parent_host);
    else {
      parents.append(",");
      parents.append(parent_host);
      it->set("parents", parents);
    }
  }
  return;
}

/**
 *  Generate a command list.
 *
 *  @param[in] count    Number of commands to generate.
 */
void centengine_config::generate_commands(int count) {
  static int id(0);

  for (int i(0); i < count; ++i) {
    // Generate name.
    std::string name;
    {
      std::ostringstream oss;
      oss << ++id;
      name = oss.str();
    }

    // Object.
    centengine_object new_command(centengine_object::command_type);
    new_command.set("command_name", name);
    _commands.push_back(new_command);
  }

  return;
}

/**
 *  Generate a host list.
 *
 *  @param[in] count  Number of hosts to generate.
 */
void centengine_config::generate_hosts(int count) {
  static int id(0);

  for (int i(0); i < count; ++i) {
    // Generate name.
    std::string name;
    {
      std::ostringstream oss;
      oss << ++id;
      name = oss.str();
    }

    // Object.
    centengine_object new_host(centengine_object::host_type);
    new_host.set("host_name", name);
    new_host.set("_HOST_ID", name);
    _hosts.push_back(new_host);
  }

  return;
}

/**
 *  Generate a service list.
 *
 *  @param[in] services_per_host  Number of service per host to
 *                                generate.
 */
void centengine_config::generate_services(int services_per_host) {
  static int id(0);

  for (objlist::iterator it(++_hosts.begin()), end(_hosts.end()); it != end;
       ++it) {
    for (int i(0); i < services_per_host; ++i) {
      // Generate service description.
      std::string description;
      {
        std::ostringstream oss;
        oss << ++id;
        description = oss.str();
      }

      // Object.
      centengine_object new_service(centengine_object::service_type);
      new_service.set("service_description", description);
      new_service.set("host_name", it->get("host_name"));
      new_service.set("_SERVICE_ID", description);
      _services.push_back(new_service);
    }
  }

  return;
}

/**
 *  Get cbmod configuration file.
 *
 *  @return cbmod configuration file.
 */
std::string const& centengine_config::get_cbmod_cfg_file() const {
  return (_cbmod_cfg);
}

/**
 *  Get command list.
 *
 *  @return Modifiable command list.
 */
centengine_config::objlist& centengine_config::get_commands() {
  return (_commands);
}

/**
 *  Get command list.
 *
 *  @return Non-modifiable command list.
 */
centengine_config::objlist const& centengine_config::get_commands() const {
  return (_commands);
}

/**
 *  Get contact list.
 *
 *  @return Modifiable contact list.
 */
centengine_config::objlist& centengine_config::get_contacts() {
  return (_contacts);
}

/**
 *  Get contact list.
 *
 *  @return Non-modifiable contact list.
 */
centengine_config::objlist const& centengine_config::get_contacts() const {
  return (_contacts);
}

/**
 *  Get global directives.
 *
 *  @return Directives map.
 */
std::map<std::string, std::string> const& centengine_config::get_directives()
    const {
  return (_directives);
}

/**
 *  Get host list.
 *
 *  @return Modifiable host list.
 */
centengine_config::objlist& centengine_config::get_hosts() {
  return (_hosts);
}

/**
 *  Get host list.
 *
 *  @return Non-modifiable host list.
 */
centengine_config::objlist const& centengine_config::get_hosts() const {
  return (_hosts);
}

/**
 *  Get host dependency list.
 *
 *  @return Modifiable host dependency list.
 */
centengine_config::objlist& centengine_config::get_host_dependencies() {
  return (_host_deps);
}

/**
 *  Get host dependency list.
 *
 *  @return Non-modifiable host dependency list.
 */
centengine_config::objlist const& centengine_config::get_host_dependencies()
    const {
  return (_host_deps);
}

/**
 *  Get host group list.
 *
 *  @return Modifiable host group list.
 */
centengine_config::objlist& centengine_config::get_host_groups() {
  return (_host_groups);
}

/**
 *  Get host group list.
 *
 *  @return Non-modifiable host group list.
 */
centengine_config::objlist const& centengine_config::get_host_groups() const {
  return (_host_groups);
}

/**
 *  Get service list.
 *
 *  @return Modifiable service list.
 */
centengine_config::objlist& centengine_config::get_services() {
  return (_services);
}

/**
 *  Get service list.
 *
 *  @return Non-modifiable service list.
 */
centengine_config::objlist const& centengine_config::get_services() const {
  return (_services);
}

/**
 *  Get service dependency list.
 *
 *  @return Modifiable service dependency list.
 */
centengine_config::objlist& centengine_config::get_service_dependencies() {
  return (_service_deps);
}

/**
 *  Get service dependency list.
 *
 *  @return Non-modifiable service dependency list.
 */
centengine_config::objlist const& centengine_config::get_service_dependencies()
    const {
  return (_service_deps);
}

/**
 *  Get service group list.
 *
 *  @return Modifiable host group list.
 */
centengine_config::objlist& centengine_config::get_service_groups() {
  return (_service_groups);
}

/**
 *  Get service group list.
 *
 *  @return Non-modifiable host group list.
 */
centengine_config::objlist const& centengine_config::get_service_groups()
    const {
  return (_service_groups);
}

/**
 *  Get timeperiod list.
 *
 *  @return Modifiable timeperiod list.
 */
centengine_config::objlist& centengine_config::get_timeperiods() {
  return (_timeperiods);
}

/**
 *  Get timeperiod list.
 *
 *  @return Non-modifiable timeperiod list.
 */
centengine_config::objlist const& centengine_config::get_timeperiods() const {
  return (_timeperiods);
}

/**
 *  Set a dependency between two services.
 *
 *  @param[in] dependent_host     Dependent host.
 *  @param[in] dependent_service  Dependent service.
 *  @param[in] depended_host      Depended host.
 *  @param[in] depended_service   Depended service.
 */
void centengine_config::service_depends_on(
    std::string const& dependent_host,
    std::string const& dependent_service,
    std::string const& depended_host,
    std::string const& depended_service) {
  centengine_object obj(centengine_object::servicedependency_type);
  obj.set("dependent_host_name", dependent_host);
  obj.set("dependent_service_description", dependent_service);
  obj.set("host_name", depended_host);
  obj.set("service_description", depended_service);
  _service_deps.push_back(obj);
  return;
}

/**
 *  Set cbmod configuration file.
 *
 *  @param[in] cfg_path  Path to the configuration file.
 */
void centengine_config::set_cbmod_cfg_file(std::string const& cfg_path) {
  _cbmod_cfg = cfg_path;
  return;
}

/**
 *  Set a global directive.
 *
 *  @param[in] directive  Directive name.
 *  @param[in] value      Directive value.
 */
void centengine_config::set_directive(std::string const& directive,
                                      std::string const& value) {
  _directives[directive] = value;
  return;
}
