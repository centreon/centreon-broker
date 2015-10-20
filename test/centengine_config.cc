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

#include <cstring>
#include <sstream>
#include "com/centreon/engine/objects.hh"
#include "test/centengine_config.hh"
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
  set_directive("state_retention_file", "");
  set_directive("check_result_reaper_frequency", "2");
  // Deprecated in Centreon Engine 2.x.
  set_directive("accept_passive_host_checks", "1");
  set_directive("accept_passive_service_checks", "1");
  set_directive("check_result_path", ".");
  set_directive("event_broker_options", "-1");
  set_directive("execute_host_checks", "1");
  set_directive("execute_service_checks", "1");
  set_directive("interval_length", MONITORING_ENGINE_INTERVAL_LENGTH_STR);
  set_directive("max_service_check_spread", "1");
  set_directive("max_concurrent_checks", "200");
  set_directive("service_inter_check_delay_method", "s");
  set_directive("sleep_time", "0.01");
  set_directive("status_file", "monitoring_engine_status.dat");
  set_directive("service_inter_check_delay_method", "n");
  set_directive("host_inter_check_delay_method", "n");
}

/**
 *  Destructor.
 */
centengine_config::~centengine_config() {
  // Free commands.
  for (std::list<command>::iterator
         it(_commands.begin()),
         end(_commands.end());
       it != end;
       ++it) {
    delete [] it->name;
    delete [] it->command_line;
  }
  _commands.clear();

  // Free hosts.
  for (std::list<host>::iterator it(_hosts.begin()), end(_hosts.end());
       it != end;
       ++it) {
    delete [] it->name;
    delete [] it->alias;
    delete [] it->host_check_command;
    for (hostsmember* child(it->child_hosts); child; ) {
      hostsmember* to_delete(child);
      child = child->next;
      delete [] to_delete->host_name;
      delete to_delete;
    }
    for (hostsmember* parent(it->parent_hosts); parent; ) {
      hostsmember* to_delete(parent);
      parent = parent->next;
      delete [] to_delete->host_name;
      delete to_delete;
    }
    for (customvariablesmember* cvar(it->custom_variables); cvar; ) {
      customvariablesmember* to_delete(cvar);
      cvar = cvar->next;
      delete [] to_delete->variable_name;
      delete [] to_delete->variable_value;
      delete to_delete;
    }
  }
  _hosts.clear();

  // Free host dependencies.
  for (std::list<hostdependency>::iterator
         it(_host_deps.begin()),
         end(_host_deps.end());
       it != end;
       ++it) {
    delete [] it->dependency_period;
    delete [] it->dependent_host_name;
    delete [] it->host_name;
  }
  _host_deps.clear();

  // Free services.
  for (std::list<service>::iterator
         it(_services.begin()),
         end(_services.end());
       it != end;
       ++it) {
    delete [] it->description;
    delete [] it->host_name;
    delete [] it->service_check_command;
    for (customvariablesmember* cvar(it->custom_variables); cvar; ) {
      customvariablesmember* to_delete(cvar);
      cvar = cvar->next;
      delete [] to_delete->variable_name;
      delete [] to_delete->variable_value;
      delete to_delete;
    }
  }
  _services.clear();

  // Free service dependencies.
  for (std::list<servicedependency>::iterator
         it(_service_deps.begin()),
         end(_service_deps.end());
       it != end;
       ++it) {
    delete [] it->dependency_period;
    delete [] it->dependent_host_name;
    delete [] it->dependent_service_description;
    delete [] it->host_name;
    delete [] it->service_description;
  }
  _service_deps.clear();
}

/**
 *  Find a host in the host list.
 *
 *  @param[in] host_name  Host name.
 *
 *  @return Iterator to the matching entry if found,
 *          get_hosts().end() otherwise.
 */
std::list<host>::iterator centengine_config::find_host(
                                               char const* host_name) {
  for (std::list<host>::iterator it(_hosts.begin()), end(_hosts.end());
       it != end;
       ++it)
    if ((!host_name && !it->name)
        || (host_name && it->name && !strcmp(host_name, it->name)))
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
std::list<service>::iterator centengine_config::find_service(
                               char const* host_name,
                               char const* service_description) {
  for (std::list<service>::iterator
         it(_services.begin()),
         end(_services.end());
       it != end;
       ++it)
    // Case 1 : all null pointers.
    if ((!host_name
         && !service_description
         && !it->host_name
         && !it->description)
        // Case 2 : host name is null, description is set.
        || (!host_name
            && service_description
            && !it->host_name
            && it->description
            && !strcmp(service_description, it->description))
        // Case 3 : host name is set, description is null.
        || (host_name
            && !service_description
            && it->host_name
            && !it->description
            && !strcmp(host_name, it->host_name))
        // Case 4 : all set.
        || (host_name
            && service_description
            && it->host_name
            && it->description
            && !strcmp(host_name, it->host_name)
            && !strcmp(service_description, it->description)))
      return (it);
  return (_services.end());
}

/**
 *  Set a dependency between two hosts.
 *
 *  @param[in] dependent_host  Name of dependent host.
 *  @param[in] depended_host   Name of depended host.
 */
void centengine_config::host_depends_on(
                          char const* dependent_host,
                          char const* depended_host) {
  hostdependency dep;
  memset(&dep, 0, sizeof(dep));
  dep.dependent_host_name = new char[strlen(dependent_host) + 1];
  strcpy(dep.dependent_host_name, dependent_host);
  dep.host_name = new char[strlen(depended_host) + 1];
  strcpy(dep.host_name, depended_host);
  return ;
}

/**
 *  Set a parenting relationship between two hosts.
 *
 *  @param[in] parent_host  Parent host.
 *  @param[in] child_host   Child host.
 */
void centengine_config::host_parent_of(
                          char const* parent_host,
                          char const* child_host) {
  // Find hosts.
  std::list<host>::iterator parent(find_host(parent_host));
  std::list<host>::iterator child(find_host(child_host));
  if ((parent != _hosts.end())
      && (child != _hosts.end())) {
    // Parent is parent of child.
    hostsmember** parent_member(&parent->child_hosts);
    while (*parent_member)
      parent_member = &(*parent_member)->next;
    *parent_member = new hostsmember;
    memset(*parent_member, 0, sizeof(**parent_member));
    (*parent_member)->host_name = new char[strlen(child->name) + 1];
    strcpy((*parent_member)->host_name, child->name);

    // Child is child of parent.
    hostsmember** child_member(&child->parent_hosts);
    while (*child_member)
      child_member = &(*child_member)->next;
    *child_member = new hostsmember;
    memset(*child_member, 0, sizeof(**child_member));
    (*child_member)->host_name = new char[strlen(parent->name) + 1];
    strcpy((*child_member)->host_name, parent->name);
  }
  return ;
}

/**
 *  Generate a command list.
 *
 *  @param[in] count    Number of commands to generate.
 */
void centengine_config::generate_commands(int count) {
  static int id(0);

  for (int i(0); i < count; ++i) {
    // Create new command.
    command new_command;
    memset(&new_command, 0, sizeof(new_command));

    // Generate name.
    std::string name;
    {
      std::ostringstream oss;
      oss << ++id;
      name = oss.str();
    }

    // Set command name.
    new_command.name = new char[name.size() + 1];
    strcpy(new_command.name, name.c_str());

    // Add to list.
    _commands.push_back(new_command);
  }

  return ;
}

/**
 *  Generate a host list.
 *
 *  @param[in] count  Number of hosts to generate.
 */
void centengine_config::generate_hosts(int count) {
  static int id(0);

  for (int i(0); i < count; ++i) {
    // Create new host.
    host new_host;
    memset(&new_host, 0, sizeof(new_host));

    // Generate name.
    std::string name;
    {
      std::ostringstream oss;
      oss << ++id;
      name = oss.str();
    }

    // Set host name.
    new_host.name = new char[name.size() + 1];
    strcpy(new_host.name, name.c_str());

    // Set some default properties.
    new_host.checks_enabled = 1;

    // Add to list.
    _hosts.push_back(new_host);

    // Set host ID.
    set_host_custom_variable(name.c_str(), "HOST_ID", name.c_str());
  }

  return ;
}

/**
 *  Generate a service list.
 *
 *  @param[in] services_per_host  Number of service per host to
 *                                generate.
 */
void centengine_config::generate_services(int services_per_host) {
  static int id(0);

  for (std::list<host>::iterator it(_hosts.begin()), end(_hosts.end());
       it != end;
       ++it) {
    for (int i(0); i < services_per_host; ++i) {
      // Create new service.
      service new_service;
      memset(&new_service, 0, sizeof(new_service));

      // Generate service description.
      std::string description;
      {
        std::ostringstream oss;
        oss << ++id;
        description = oss.str();
      }

      // Set service description.
      new_service.description = new char[description.size() + 1];
      strcpy(new_service.description, description.c_str());

      // Set host.
      new_service.host_name = new char[strlen(it->name) + 1];
      strcpy(new_service.host_name, it->name);

      // Set some default properties.
      new_service.checks_enabled = 1;

      // Add to list.
      _services.push_back(new_service);

      // Set service ID.
      set_service_custom_variable(
        new_service.host_name,
        new_service.description,
        "SERVICE_ID",
        description.c_str());

      // Set host ID.
      set_service_custom_variable(
        new_service.host_name,
        new_service.description,
        "HOST_ID",
        it->name);
    }
  }

  return ;
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
std::list<command>& centengine_config::get_commands() {
  return (_commands);
}

/**
 *  Get command list.
 *
 *  @return Non-modifiable command list.
 */
std::list<command> const& centengine_config::get_commands() const {
  return (_commands);
}

/**
 *  Get global directives.
 *
 *  @return Directives map.
 */
std::map<std::string, std::string> const& centengine_config::get_directives() const {
  return (_directives);
}

/**
 *  Get host list.
 *
 *  @return Modifiable host list.
 */
std::list<host>& centengine_config::get_hosts() {
  return (_hosts);
}

/**
 *  Get host list.
 *
 *  @return Non-modifiable host list.
 */
std::list<host> const& centengine_config::get_hosts() const {
  return (_hosts);
}

/**
 *  Get host dependency list.
 *
 *  @return Modifiable host dependency list.
 */
std::list<hostdependency>& centengine_config::get_host_dependencies() {
  return (_host_deps);
}

/**
 *  Get host dependency list.
 *
 *  @return Non-modifiable host dependency list.
 */
std::list<hostdependency> const& centengine_config::get_host_dependencies() const {
  return (_host_deps);
}

/**
 *  Get service list.
 *
 *  @return Modifiable service list.
 */
std::list<service>& centengine_config::get_services() {
  return (_services);
}

/**
 *  Get service list.
 *
 *  @return Non-modifiable service list.
 */
std::list<service> const& centengine_config::get_services() const {
  return (_services);
}

/**
 *  Get service dependency list.
 *
 *  @return Modifiable service dependency list.
 */
std::list<servicedependency>& centengine_config::get_service_dependencies() {
  return (_service_deps);
}

/**
 *  Get service dependency list.
 *
 *  @return Non-modifiable service dependency list.
 */
std::list<servicedependency> const& centengine_config::get_service_dependencies() const {
  return (_service_deps);
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
                          char const* dependent_host,
                          char const* dependent_service,
                          char const* depended_host,
                          char const* depended_service) {
  servicedependency dep;
  memset(&dep, 0, sizeof(dep));
  dep.dependent_host_name = new char[strlen(dependent_host) + 1];
  strcpy(dep.dependent_host_name, dependent_host);
  dep.dependent_service_description
    = new char[strlen(dependent_service) + 1];
  strcpy(dep.dependent_service_description, dependent_service);
  dep.host_name = new char[strlen(depended_host) + 1];
  strcpy(dep.host_name, depended_host);
  dep.service_description = new char[strlen(depended_service) + 1];
  strcpy(dep.service_description, depended_service);
  return ;
}

/**
 *  Set cbmod configuration file.
 *
 *  @param[in] cfg_path  Path to the configuration file.
 */
void centengine_config::set_cbmod_cfg_file(
                          std::string const& cfg_path) {
  _cbmod_cfg = cfg_path;
  return ;
}

/**
 *  Set a global directive.
 *
 *  @param[in] directive  Directive name.
 *  @param[in] value      Directive value.
 */
void centengine_config::set_directive(
                          std::string const& directive,
                          std::string const& value) {
  _directives[directive] = value;
  return ;
}

/**
 *  Set host custom variable.
 *
 *  @param[in] host_name  Host name.
 *  @param[in] var_name   Variable name.
 *  @param[in] var_value  Variable value.
 */
void centengine_config::set_host_custom_variable(
                          char const* host_name,
                          char const* var_name,
                          char const* var_value) {
  std::list<host>::iterator it;
  it = find_host(host_name);
  if (it != _hosts.end())
    _set_custom_variable(&it->custom_variables, var_name, var_value);
  return ;
}

/**
 *  Set service custom variable.
 *
 *  @param[in] host_name            Host name.
 *  @param[in] service_description  Service description.
 *  @param[in] var_name             Variable name.
 *  @param[in] var_value            Variable value.
 */
void centengine_config::set_service_custom_variable(
                          char const* host_name,
                          char const* service_description,
                          char const* var_name,
                          char const* var_value) {
  std::list<service>::iterator it;
  it = find_service(host_name, service_description);
  if (it != _services.end())
    _set_custom_variable(&it->custom_variables, var_name, var_value);
  return ;
}

/**
 *  Custom variable modification core function.
 *
 *  @param[in,out] vars       Custom variable list.
 *  @param[in]     var_name   Variable name.
 *  @param[in]     var_value  Variable value.
 */
void centengine_config::_set_custom_variable(
                          customvariablesmember** vars,
                          char const* var_name,
                          char const* var_value) {
  while (*vars && strcmp((*vars)->variable_name, var_name))
    vars = &(*vars)->next;
  if (!*vars) {
    *vars = new customvariablesmember;
    memset(*vars, 0, sizeof(**vars));
    (*vars)->variable_name = new char[strlen(var_name) + 1];
    strcpy((*vars)->variable_name, var_name);
  }
  else {
    delete [] (*vars)->variable_value;
    (*vars)->variable_value = NULL;
  }
  (*vars)->variable_value = new char[strlen(var_value) + 1];
  strcpy((*vars)->variable_value, var_value);
  return ;
}
