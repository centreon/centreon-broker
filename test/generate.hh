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

#ifndef TEST_GENERATE_HH
#  define TEST_GENERATE_HH

#  include <list>
#  include "com/centreon/engine/objects.hh"

void depends_on(hostdependency& dep, host& dependent, host& depended);
void depends_on(
       servicedependency& dep,
       service& dependent,
       service& depended);
void free_commands(std::list<command>& commands);
void free_hosts(std::list<host>& hosts);
void free_host_dependencies(std::list<hostdependency>& deps);
void free_host_groups(std::list<hostgroup>& host_groups);
void free_services(std::list<service>& services);
void free_service_dependencies(std::list<servicedependency>& deps);
void free_service_groups(std::list<servicegroup>& service_groups);
void generate_commands(
       std::list<command>& commands,
       unsigned int count);
void generate_hosts(
       std::list<host>& hosts,
       unsigned int count);
void generate_host_dependencies(
       std::list<hostdependency>& deps,
       unsigned int count);
void generate_host_groups(
       std::list<hostgroup>& host_groups,
       unsigned int count);
void generate_services(
       std::list<service>& services,
       std::list<host>& hosts,
       unsigned int services_per_host);
void generate_service_dependencies(
       std::list<servicedependency>& deps,
       unsigned int count);
void generate_service_groups(
       std::list<servicegroup>& service_groups,
       unsigned int count);
void link(host& h, hostgroup& hg);
void link(service& s, servicegroup& sg);
void parent_of(host& parent, host& child);
void set_custom_variable(
       host& h,
       char const* name,
       char const* value);
void set_custom_variable(
       service& s,
       char const* name,
       char const* value);

#endif // !TEST_GENERATE_HH
