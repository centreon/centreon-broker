/*
** Copyright 2012-2013 Merethis
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

#include <cstdlib>
#include <cstring>
#include <sstream>
#include "test/generate.hh"

/**
 *  Create a host dependency.
 *
 *  @param[out] dep       Host dependency.
 *  @param[in]  dependent Dependent host.
 *  @param[in]  depended  Host that is depended by the dependent host.
 */
void depends_on(hostdependency& dep, host& dependent, host& depended) {
  dep.dependent_host_name = new char[strlen(dependent.name) + 1];
  strcpy(dep.dependent_host_name, dependent.name);
  dep.host_name = new char[strlen(depended.name) + 1];
  strcpy(dep.host_name, depended.name);
  return ;
}

/**
 *  Create a service dependency.
 *
 *  @param[out] dep       Service dependency.
 *  @param[in]  dependent Dependent service.
 *  @param[in]  depended  Service that is depended by the dependent
 *                        service.
 */
void depends_on(
       servicedependency& dep,
       service& dependent,
       service& depended) {
  dep.dependent_host_name = new char[strlen(dependent.host_name) + 1];
  strcpy(dep.dependent_host_name, dependent.host_name);
  dep.dependent_service_description
    = new char[strlen(dependent.description) + 1];
  strcpy(dep.dependent_service_description, dependent.description);
  dep.host_name = new char[strlen(depended.host_name) + 1];
  strcpy(dep.host_name, depended.host_name);
  dep.service_description = new char[strlen(depended.description) + 1];
  strcpy(dep.service_description, depended.description);
  return ;
}

/**
 *  Free the command list.
 *
 *  @param[in,out] commands Commands to free.
 */
void free_commands(std::list<command>& commands) {
  for (std::list<command>::iterator
         it(commands.begin()),
         end(commands.end());
       it != end;
       ++it) {
    delete [] it->name;
    delete [] it->command_line;
  }
  commands.clear();
  return ;
}

/**
 *  Free the host list.
 *
 *  @param[in,out] hosts Hosts to free.
 */
void free_hosts(std::list<host>& hosts) {
  for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
       it != end;
       ++it) {
    delete [] it->name;
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
  hosts.clear();
  return ;
}

/**
 *  Free the host dependency list.
 *
 *  @param[in,out] deps Host dependencies to free.
 */
void free_host_dependencies(std::list<hostdependency>& deps) {
  for (std::list<hostdependency>::iterator
         it(deps.begin()),
         end(deps.end());
       it != end;
       ++it) {
    delete [] it->dependency_period;
    delete [] it->dependent_host_name;
    delete [] it->host_name;
  }
  deps.clear();
  return ;
}

/**
 *  Free the host group list.
 *
 *  @param[in,out] host_groups Host groups to free.
 */
void free_host_groups(std::list<hostgroup>& host_groups) {
  for (std::list<hostgroup>::iterator
         it(host_groups.begin()),
         end(host_groups.end());
       it != end;
       ++it) {
    delete [] it->group_name;
    delete [] it->alias;
    delete [] it->notes;
    delete [] it->notes_url;
    delete [] it->action_url;
    for (hostsmember* m(it->members); m; ) {
      hostsmember* to_delete(m);
      m = m->next;
      delete [] to_delete->host_name;
      delete to_delete;
    }
  }
  host_groups.clear();
  return ;
}

/**
 *  Free the service list.
 *
 *  @param[in,out] services Services to free.
 */
void free_services(std::list<service>& services) {
  for (std::list<service>::iterator
         it(services.begin()),
         end(services.end());
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
  services.clear();
  return ;
}

/**
 *  Free the service dependency list.
 *
 *  @param[in,out] deps Service dependencies to free.
 */
void free_service_dependencies(std::list<servicedependency>& deps) {
  for (std::list<servicedependency>::iterator
         it(deps.begin()),
         end(deps.end());
       it != end;
       ++it) {
    delete [] it->dependency_period;
    delete [] it->dependent_host_name;
    delete [] it->dependent_service_description;
    delete [] it->host_name;
    delete [] it->service_description;
  }
  deps.clear();
  return ;
}

/**
 *  Free the service group list.
 *
 *  @param[in,out] service_groups Service groups to free.
 */
void free_service_groups(std::list<servicegroup>& service_groups) {
  for (std::list<servicegroup>::iterator
         it(service_groups.begin()),
         end(service_groups.end());
       it != end;
       ++it) {
    delete [] it->group_name;
    delete [] it->alias;
    delete [] it->notes;
    delete [] it->notes_url;
    delete [] it->action_url;
    for (servicesmember* m(it->members); m; ) {
      servicesmember* to_delete(m);
      m = m->next;
      delete [] to_delete->host_name;
      delete [] to_delete->service_description;
      delete to_delete;
    }
  }
  service_groups.clear();
  return ;
}

/**
 *  Generate a command list.
 *
 *  @param[out] commands Generated command list.
 *  @param[in]  count    Number of commands to generate.
 */
void generate_commands(
        std::list<command>& commands,
        unsigned int count) {
  static unsigned int id(0);

  for (unsigned int i(0); i < count; ++i) {
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
    commands.push_back(new_command);
  }

  return ;
}

/**
 *  Generate a host list.
 *
 *  @param[out] hosts Generated host list.
 *  @param[in]  count Number of hosts to generate.
 */
void generate_hosts(
       std::list<host>& hosts,
       unsigned int count) {
  static unsigned int id(0);

  for (unsigned int i(0); i < count; ++i) {
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
    hosts.push_back(new_host);
  }

  return ;
}

/**
 *  Generate a host dependency list.
 *
 *  @param[out] deps  Generated host dependency list.
 *  @param[in]  count Number of dependencies to generate.
 */
void generate_host_dependencies(
       std::list<hostdependency>& deps,
       unsigned int count) {
  for (unsigned int i(0); i < count; ++i) {
    hostdependency dep;
    memset(&dep, 0, sizeof(dep));
    deps.push_back(dep);
  }
  return ;
}

/**
 *  Generate a host group list.
 *
 *  @param[out] host_groups Generated host group list.
 *  @param[in]  count       Number of groups to generate.
 */
void generate_host_groups(
       std::list<hostgroup>& host_groups,
       unsigned int count) {
  static unsigned int id(0);

  for (unsigned int i(0); i < count; ++i) {
    // Create new host group.
    hostgroup new_group;
    memset(&new_group, 0, sizeof(new_group));

    // Generate name.
    std::string name;
    {
      std::ostringstream oss;
      oss << ++id;
      name = oss.str();
    }

    // Set group name.
    new_group.group_name = new char[name.size() + 1];
    strcpy(new_group.group_name, name.c_str());

    // Add to list.
    host_groups.push_back(new_group);
  }

  return ;
}

/**
 *  Generate a service list.
 *
 *  @param[out] services          Generated service list.
 *  @param[in]  hosts             Hosts.
 *  @param[in]  services_per_host Number of service per host to
 *                                generate.
 */
void generate_services(
       std::list<service>& services,
       std::list<host>& hosts,
       unsigned int services_per_host) {
  static unsigned int id(0);

  for (std::list<host>::iterator it(hosts.begin()), end(hosts.end());
       it != end;
       ++it) {
    for (unsigned int i(0); i < services_per_host; ++i) {
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
      services.push_back(new_service);
    }
  }

  return ;
}

/**
 *  Generate a service dependency list.
 *
 *  @param[out] deps  Generated service dependency list.
 *  @param[in]  count Number of dependencies to generate.
 */
void generate_service_dependencies(
       std::list<servicedependency>& deps,
       unsigned int count) {
  for (unsigned int i(0); i < count; ++i) {
    servicedependency dep;
    memset(&dep, 0, sizeof(dep));
    deps.push_back(dep);
  }
  return ;
}

/**
 *  Generate a service group list.
 *
 *  @param[out] service_groups Generated service group list.
 *  @param[in]  count          Number of groups to generate.
 */
void generate_service_groups(
       std::list<servicegroup>& service_groups,
       unsigned int count) {
  static unsigned int id(0);

  for (unsigned int i(0); i < count; ++i) {
    // Create new service group.
    servicegroup new_group;
    memset(&new_group, 0, sizeof(new_group));

    // Generate name.
    std::string name;
    {
      std::ostringstream oss;
      oss << ++id;
      name = oss.str();
    }

    // Set group name.
    new_group.group_name = new char[name.size() + 1];
    strcpy(new_group.group_name, name.c_str());

    // Add to list.
    service_groups.push_back(new_group);
  }

  return ;
}

/**
 *  Link a host to a host group.
 *
 *  @param[in,out] h  Host.
 *  @param[in,out] hg Host group.
 */
void link(host& h, hostgroup& hg) {
  // Find insertion point.
  hostsmember** m;
  for (m = &hg.members; *m; m = &((*m)->next))
    ;

  // Create link.
  *m = new hostsmember;
  memset(*m, 0, sizeof(**m));

  // Set host name.
  (*m)->host_name = new char[strlen(h.name) + 1];
  strcpy((*m)->host_name, h.name);

  return ;
}

/**
 *  Link a service to a service group.
 *
 *  @param[in,out] s  Service.
 *  @param[in,out] sg Service group.
 */
void link(service& s, servicegroup& sg) {
  // Find insertion point.
  servicesmember** m;
  for (m = &sg.members; *m; m = &((*m)->next))
    ;

  // Create link.
  *m = new servicesmember;
  memset(*m, 0, sizeof(**m));

  // Set host name.
  (*m)->host_name = new char[strlen(s.host_name) + 1];
  strcpy((*m)->host_name, s.host_name);

  // Set service description.
  (*m)->service_description = new char[strlen(s.description) + 1];
  strcpy((*m)->service_description, s.description);

  return ;
}

/**
 *  Declare a parenting relationship between two hosts.
 *
 *  @param[in,out] parent Parent host.
 *  @param[in,out] child  Child host.
 */
void parent_of(host& parent, host& child) {
  // Parent is parent of child.
  hostsmember** parent_member(&parent.child_hosts);
  *parent_member = new hostsmember;
  memset(*parent_member, 0, sizeof(**parent_member));
  (*parent_member)->host_name = new char[strlen(child.name) + 1];
  strcpy((*parent_member)->host_name, child.name);

  // Child is child of parent.
  hostsmember** child_member(&child.parent_hosts);
  *child_member = new hostsmember;
  memset(*child_member, 0, sizeof(**child_member));
  (*child_member)->host_name = new char[strlen(parent.name) + 1];
  strcpy((*child_member)->host_name, parent.name);

  return ;
}

/**
 *  Custom variable modification core function.
 *
 *  @param[in,out] vars  Custom variable list.
 *  @param[in]     name  Variable name.
 *  @param[in]     value Variable value.
 */
static void set_custom_variable(
              customvariablesmember** vars,
              char const* name,
              char const* value) {
  while (*vars && strcmp((*vars)->variable_name, name))
    vars = &(*vars)->next;
  if (!*vars) {
    *vars = new customvariablesmember;
    memset(*vars, 0, sizeof(**vars));
    (*vars)->variable_name = new char[strlen(name) + 1];
    strcpy((*vars)->variable_name, name);
  }
  else {
    delete [] (*vars)->variable_value;
    (*vars)->variable_value = NULL;
  }
  (*vars)->variable_value = new char[strlen(value) + 1];
  strcpy((*vars)->variable_value, value);
  return ;
}

/**
 *  Set host custom variable.
 *
 *  @param[in,out] h     Target host.
 *  @param[in]     name  Variable name.
 *  @param[in]     value Variable value.
 */
void set_custom_variable(
       host& h,
       char const* name,
       char const* value) {
  set_custom_variable(&h.custom_variables, name, value);
  return ;
}

/**
 *  Set service custom variable.
 *
 *  @param[in,out] s     Target service.
 *  @param[in]     name  Variable name.
 *  @param[in]     value Variable value.
 */
void set_custom_variable(
       service& s,
       char const* name,
       char const* value) {
  set_custom_variable(&s.custom_variables, name, value);
  return ;
}
