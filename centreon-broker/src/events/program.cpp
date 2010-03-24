/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include "events/program.h"

using namespace Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy internal data of the Program object to the current instance.
 *
 *  Copy data defined within the Program class. This method is used by the copy
 *  constructor and the assignment operator.
 *
 *  \param[in] program Object to copy data from.
 */
void Program::InternalCopy(const Program& program)
{
  this->daemon_mode   = program.daemon_mode;
  this->instance_id   = program.instance_id;
  this->instance_name = program.instance_name;
  this->is_running    = program.is_running;
  this->pid           = program.pid;
  this->program_end   = program.program_end;
  this->program_start = program.program_start;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Program default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
Program::Program()
  : daemon_mode(false),
    instance_id(0),
    is_running(false),
    pid(0),
    program_end(0),
    program_start(0) {}

/**
 *  \brief Program copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  \param[in] program Object to copy data from.
 */
Program::Program(const Program& program) : Event(program)
{
  this->InternalCopy(program);
}

/**
 *  Program destructor.
 */
Program::~Program() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  \param[in] program Object to copy data from.
 */
Program& Program::operator=(const Program& program)
{
  this->Event::operator=(program);
  this->InternalCopy(program);
  return (*this);
}

/**
 *  \brief Get the type of the event (Event::PROGRAM).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  \return Event::PROGRAM
 */
int Program::GetType() const
{
  return (Event::PROGRAM);
}
