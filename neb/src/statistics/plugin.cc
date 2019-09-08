/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/neb/statistics/plugin.hh"

using namespace com::centreon::broker::neb::statistics;

/**
 *  Constructor.
 *
 *  @param[in] name The plugin name.
 */
plugin::plugin(std::string const& name) : _name(name) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
plugin::plugin(plugin const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
plugin::~plugin() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
plugin& plugin::operator=(plugin const& right) {
  if (this != &right) {
    _name = right._name;
  }
  return (*this);
}

/**
 *  Get the plugin name.
 *
 *  @return The plugin name.
 */
std::string const& plugin::name() const throw() {
  return (_name);
}
