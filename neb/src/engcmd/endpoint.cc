/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/neb/engcmd/endpoint.hh"
#include "com/centreon/broker/neb/engcmd/engine_command.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb::engcmd;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the endpoint.
 */
endpoint::endpoint(
            std::string const& name,
            std::string const& command_module_path)
  : io::endpoint(false),
    _name(name),
    _command_module_path(command_module_path) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
endpoint::endpoint(endpoint const& other)
  : io::endpoint(false) {
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
misc::shared_ptr<io::stream> endpoint::open() {
  return (misc::make_shared(new engine_command(_name, _command_module_path)));
}
