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

#ifndef CCB_WATCHDOG_CONFIGURATION_HH
#define CCB_WATCHDOG_CONFIGURATION_HH

#include <map>
#include <string>

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/watchdog/instance_configuration.hh"

CCB_BEGIN()

namespace watchdog {
/**
 *  @class configuration configuration.hh
 * "com/centreon/broker/watchdog/configuration.hh"
 *  @brief Configuration of the watchdog.
 */
class configuration {
 public:
  typedef std::map<std::string, instance_configuration> instance_map;

  configuration();
  ~configuration();
  configuration(std::string const& log_filename,
                std::map<std::string, instance_configuration> const& instances);
  configuration(configuration const& other);
  configuration& operator=(configuration const& other);

  std::string const& get_log_filename() const throw();
  instance_map const& get_instances_configuration() const throw();
  instance_configuration get_instance_configuration(
      std::string const& name) const;
  bool instance_exists(std::string const& name) const throw();

 private:
  std::string _log_filename;
  instance_map _instances_configuration;
};
}  // namespace watchdog

CCB_END()

#endif  // !CCB_WATCHDOG_BROKER_INSTANCE_CONFIGURATION_HH
