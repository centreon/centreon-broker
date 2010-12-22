/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "events/program.hh"

using namespace events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the program object to the current
 *         instance.
 *
 *  Copy data defined within the program class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  @param[in] program Object to copy.
 */
void program::_internal_copy(program const& p) {
  daemon_mode = p.daemon_mode;
  instance_id = p.instance_id;
  instance_name = p.instance_name;
  is_running = p.is_running;
  pid = p.pid;
  program_end = p.program_end;
  program_start = p.program_start;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
program::program()
  : daemon_mode(false),
    instance_id(0),
    is_running(false),
    pid(0),
    program_end(0),
    program_start(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] p Object to copy.
 */
program::program(program const& p) : event(p) {
  _internal_copy(p);
}

/**
 *  Destructor.
 */
program::~program() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] p Object to copy.
 */
program& program::operator=(program const& p) {
  event::operator=(p);
  _internal_copy(p);
  return (*this);
}

/**
 *  @brief Get the type of the event (event::PROGRAM).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  @return event::PROGRAM
 */
int program::get_type() const {
  return (PROGRAM);
}
