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

#include <QMutexLocker>
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
command_result::command_result() : code(0), id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
command_result::command_result(command_result const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
command_result::~command_result() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
command_result& command_result::operator=(
                                  command_result const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int command_result::type() const {
  return (command_result::static_type());
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int command_result::static_type() {
  return (io::events::data_type<io::events::internal, io::events::de_command_result>::value);
}


/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void command_result::_internal_copy(command_result const& other) {
  code = other.code;
  id = other.id;
  msg = other.msg;
  return ;
}
