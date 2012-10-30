/*
** Copyright 2012 Merethis
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
