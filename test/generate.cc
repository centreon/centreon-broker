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
  }
  hosts.clear();
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
  }
  services.clear();
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

    // Add to list.
    hosts.push_back(new_host);
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

      // Add to list.
      services.push_back(new_service);
    }
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
