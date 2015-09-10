/*
** Copyright 2012-2013 Centreon
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
