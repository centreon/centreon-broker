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
#  define CCB_TEST_CENTENGINE_CONFIG_HH

#  include <list>
#  include <map>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/engine/objects.hh"

CCB_BEGIN()

namespace                     test {
  /**
   *  Centreon Engine configuration set.
   */
  class                       centengine_config {
  public:
                              centengine_config();
                              ~centengine_config();
    std::list<host>::iterator find_host(char const* host_name);
    std::list<service>::iterator
                              find_service(
                                char const* host_name,
                                char const* service_description);
    void                      host_depends_on(
                                char const* dependent_host,
                                char const* depended_host);
    void                      host_parent_of(
                                char const* parent_host,
                                char const* child_host);
    void                      generate_commands(int count);
    void                      generate_hosts(int count);
    void                      generate_services(int services_per_host);
    std::string const&        get_cbmod_cfg_file() const;
    std::list<command>&       get_commands();
    std::list<command> const& get_commands() const;
    std::map<std::string, std::string> const&
                              get_directives() const;
    std::list<host>&          get_hosts();
    std::list<host> const&    get_hosts() const;
    std::list<hostdependency>&
                              get_host_dependencies();
    std::list<hostdependency> const&
                              get_host_dependencies() const;
    std::list<service>&       get_services();
    std::list<service> const& get_services() const;
    std::list<servicedependency>&
                              get_service_dependencies();
    std::list<servicedependency> const&
                              get_service_dependencies() const;
    void                      service_depends_on(
                                char const* dependent_host,
                                char const* dependent_service,
                                char const* depended_host,
                                char const* depended_service);
    void                      set_cbmod_cfg_file(
                                std::string const& cfg_path);
    void                      set_directive(
                                std::string const& directive,
                                std::string const& value);
    void                      set_host_custom_variable(
                                char const* host_name,
                                char const* var_name,
                                char const* var_value);
    void                      set_service_custom_variable(
                                char const* host_name,
                                char const* service_description,
                                char const* var_name,
                                char const* var_value);

  private:
                              centengine_config(
                                centengine_config const& other);
    centengine_config&        operator=(centengine_config const& other);
    void                      _set_custom_variable(
                                customvariablesmember** vars,
                                char const* var_name,
                                char const* var_value);

    std::string               _cbmod_cfg;
    std::list<command>        _commands;
    std::map<std::string, std::string>
                              _directives;
    std::list<host>           _hosts;
    std::list<hostdependency> _host_deps;
    std::list<service>        _services;
    std::list<servicedependency>
                              _service_deps;
  };
}

CCB_END()

#endif // !CCB_TEST_CENTENGINE_CONFIG_HH
