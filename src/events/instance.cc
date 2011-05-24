/*
** Copyright 2009-2011 MERETHIS
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

#include "events/instance.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the instance object to the current
 *         instance.
 *
 *  Copy data defined within the instance class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  @param[in] i Object to copy.
 */
void instance::_internal_copy(instance const& i) {
  engine = i.engine;
  id = i.id;
  is_running = i.is_running;
  name = i.name;
  pid = i.pid;
  program_end = i.program_end;
  program_start = i.program_start;
  version = i.version;
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
instance::instance()
  : id(0),
    is_running(false),
    pid(0),
    program_end(0),
    program_start(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
instance::instance(instance const& i) : event(i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
instance::~instance() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
instance& instance::operator=(instance const& i) {
  event::operator=(i);
  _internal_copy(i);
  return (*this);
}

/**
 *  @brief Get the type of the event (event::INSTANCE).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  @return event::INSTANCE
 */
int instance::get_type() const {
  return (INSTANCE);
}
