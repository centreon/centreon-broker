/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/command_file/internal.hh"
#include "com/centreon/broker/command_file/external_command.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::command_file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
external_command::external_command() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
external_command::external_command(external_command const& right)
  : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
external_command::~external_command() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
external_command& external_command::operator=(
                                      external_command const& right) {
  if (this != &right) {
    io::data::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int external_command::type() const {
  return (external_command::static_type());
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int external_command::static_type() {
  return (io::events::data_type<io::events::internal, command_file::de_command>::value);
}


/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void external_command::_internal_copy(external_command const& right) {
  command = right.command;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const external_command::entries[] = {
  mapping::entry(
    &external_command::command,
    "command",
    1),
  mapping::entry()
};

// Operations.
static io::data* new_external_command() {
  return (new external_command);
}
io::event_info::event_operations const external_command::operations = {
  &new_external_command
};
