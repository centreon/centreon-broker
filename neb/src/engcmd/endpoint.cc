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

#include "com/centreon/broker/neb/engcmd/endpoint.hh"
#include "com/centreon/broker/neb/engcmd/engine_command.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb::engcmd;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the endpoint.
 */
endpoint::endpoint(std::string const& name,
                   std::string const& command_module_path)
    : io::endpoint(false),
      _name(name),
      _command_module_path(command_module_path) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
endpoint::endpoint(endpoint const& other) : io::endpoint(false) {
  endpoint::operator=(other);
}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           Reference to this.
 */
endpoint& endpoint::operator=(endpoint const& other) {
  if (this != &other) {
    _name = other._name;
    _command_module_path = other._command_module_path;
  }
  return (*this);
}

/**
 *  Open a new stream.
 *
 *  @return  A new stream.
 */
std::shared_ptr<io::stream> endpoint::open() {
  return (std::make_shared<engine_command>(_name, _command_module_path));
}
