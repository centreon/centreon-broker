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

#ifndef CCB_WATCHDOG_INSTANCE_CONFIGURATION_HH
#define CCB_WATCHDOG_INSTANCE_CONFIGURATION_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace watchdog {
/**
 *  @class instance_configuration instance_configuration.hh
 * "com/centreon/broker/watchdog/instance_configuration.hh"
 *  @brief Configuration of a centreon broker instance.
 */
class instance_configuration {
 public:
  instance_configuration();
  instance_configuration(std::string const& name,
                         std::string const& executable,
                         std::string const& config_file,
                         bool should_run,
                         bool should_reload,
                         uint32_t seconds_per_tentative);
  ~instance_configuration();
  instance_configuration(instance_configuration const& other);
  instance_configuration& operator=(instance_configuration const& other);
  bool same_child(instance_configuration const& other) const;
  bool operator==(instance_configuration const& other) const = delete;
  bool operator!=(instance_configuration const& other) const = delete;

  bool is_empty() const throw();

  std::string const& get_name() const throw();
  std::string const& get_executable() const throw();
  std::string const& get_config_file() const throw();
  bool should_run() const throw();
  bool should_reload() const throw();

 private:
  std::string _name;
  std::string _executable;
  std::string _config_file;
  bool _run;
  bool _reload;
  uint32_t _seconds_per_tentative;
};
}  // namespace watchdog

CCB_END()

#endif  // !CCB_WATCHDOG_INSTANCE_CONFIGURATION_HH
