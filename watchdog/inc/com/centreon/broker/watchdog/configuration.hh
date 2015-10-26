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
#  define CCB_WATCHDOG_CONFIGURATION_HH

#  include <string>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/watchdog/broker_instance_configuration.hh"

CCB_BEGIN()

namespace     watchdog {
  /**
   *  @class configuration configuration.hh "com/centreon/broker/watchdog/configuration.hh"
   *  @brief Configuration of the watchdog.
   */
  class       configuration {
  public:
              configuration();
              ~configuration();
              configuration(
                std::string const& log_filename,
                std::vector<broker_instance_configuration> const& instances);
              configuration(configuration const& other);
    configuration&
              operator=(configuration const& other);

    std::string const&
              get_log_filename() const throw();
    std::vector<broker_instance_configuration> const&
              get_instances_configuration() const throw();

  private:
    std::string
              _log_filename;
    std::vector<broker_instance_configuration>
              _instances_configuration;
  };
}

CCB_END()

#endif // !CCB_WATCHDOG_BROKER_INSTANCE_CONFIGURATION_HH
