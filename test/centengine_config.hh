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

#ifndef CCB_TEST_CENTENGINE_CONFIG_HH
#define CCB_TEST_CENTENGINE_CONFIG_HH

#include <map>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "test/centengine_object.hh"

CCB_BEGIN()

namespace test {
/**
 *  Centreon Engine configuration set.
 */
class centengine_config {
 public:
  typedef std::vector<centengine_object> objlist;

  centengine_config();
  ~centengine_config();
  objlist::iterator find_host(std::string const& host_name);
  objlist::iterator find_service(std::string const& host_name,
                                 std::string const& service_description);
  void host_depends_on(std::string const& dependent_host,
                       std::string const& depended_host);
  void host_parent_of(std::string const& parent_host,
                      std::string const& child_host);
  void generate_commands(int count);
  void generate_hosts(int count);
  void generate_services(int services_per_host);
  std::string const& get_cbmod_cfg_file() const;
  objlist& get_commands();
  objlist const& get_commands() const;
  objlist& get_contacts();
  objlist const& get_contacts() const;
  std::map<std::string, std::string> const& get_directives() const;
  objlist& get_hosts();
  objlist const& get_hosts() const;
  objlist& get_host_dependencies();
  objlist const& get_host_dependencies() const;
  objlist& get_host_groups();
  objlist const& get_host_groups() const;
  objlist& get_services();
  objlist const& get_services() const;
  objlist& get_service_dependencies();
  objlist const& get_service_dependencies() const;
  objlist& get_service_groups();
  objlist const& get_service_groups() const;
  objlist& get_timeperiods();
  objlist const& get_timeperiods() const;
  void service_depends_on(std::string const& dependent_host,
                          std::string const& dependent_service,
                          std::string const& depended_host,
                          std::string const& depended_service);
  void set_cbmod_cfg_file(std::string const& cfg_path);
  void set_directive(std::string const& directive, std::string const& value);

 private:
  centengine_config(centengine_config const& other);
  centengine_config& operator=(centengine_config const& other);

  std::string _cbmod_cfg;
  objlist _commands;
  objlist _contacts;
  std::map<std::string, std::string> _directives;
  objlist _hosts;
  objlist _host_deps;
  objlist _host_groups;
  objlist _services;
  objlist _service_deps;
  objlist _service_groups;
  objlist _timeperiods;
};
}  // namespace test

CCB_END()

#endif  // !CCB_TEST_CENTENGINE_CONFIG_HH
