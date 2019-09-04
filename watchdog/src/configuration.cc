/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/watchdog/configuration.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

/**
 *  Default constructor.
 */
configuration::configuration() {}

/**
 *  Constructor.
 *
 *  @param[in] log_filename  The name of the log.
 *  @param[in] instances     The configuration of the instances centreon-broker.
 */
configuration::configuration(
    std::string const& log_filename,
    std::map<std::string, instance_configuration> const& instances)
    : _log_filename(log_filename), _instances_configuration(instances) {}

/**
 *  Destructor.
 */
configuration::~configuration() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
configuration::configuration(configuration const& other)
    : _log_filename(other._log_filename),
      _instances_configuration(other._instances_configuration) {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return  A reference to this object.
 */
configuration& configuration::operator=(configuration const& other) {
  if (this != &other) {
    _log_filename = other._log_filename;
    _instances_configuration = other._instances_configuration;
  }
  return (*this);
}

/**
 *  Get the filename of the log.
 *
 *  @return  The filename of the log.
 */
std::string const& configuration::get_log_filename() const throw() {
  return (_log_filename);
}

/**
 *  Get the configuration of the instances.
 *
 *  @return  The configuration of the instances.
 */
std::map<std::string, instance_configuration> const&
configuration::get_instances_configuration() const throw() {
  return (_instances_configuration);
}

/**
 *  Get an instance configuration, or an empty configuration instance.
 *
 *  @param[in] name  The name of the instance.
 *
 *  @return  The configuration of this instance, or an empty one.
 */
instance_configuration configuration::get_instance_configuration(
    std::string const& name) const {
  instance_map::const_iterator found = _instances_configuration.find(name);
  return (found != _instances_configuration.end() ? found->second
                                                  : instance_configuration());
}

/**
 *  Return true if an instance exist with this name.
 *
 *  @param[in] name  The name.
 *
 *  @return  True if the instance exist.
 */
bool configuration::instance_exists(std::string const& name) const throw() {
  return (_instances_configuration.find(name) !=
          _instances_configuration.end());
}
