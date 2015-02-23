/*
** Copyright 2009-2013,2015 Merethis
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

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
    is_running(true),
    pid(0),
    program_end(0),
    program_start(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
instance::instance(instance const& other) : io::data(other) {
  _internal_copy(other);
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
 *  @param[in] other  Object to copy.
 */
instance& instance::operator=(instance const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
unsigned int instance::type() const {
  return (instance::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int instance::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_instance>::value);
}

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
 *  @param[in] other  Object to copy.
 */
void instance::_internal_copy(instance const& other) {
  engine = other.engine;
  id = other.id;
  is_running = other.is_running;
  name = other.name;
  pid = other.pid;
  program_end = other.program_end;
  program_start = other.program_start;
  version = other.version;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const instance::entries[] = {
  mapping::entry(
    &instance::engine,
    "engine",
    1),
  mapping::entry(
    &instance::id,
    "instance_id",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &instance::name,
    "name",
    3),
  mapping::entry(
    &instance::is_running,
    "running",
    4),
  mapping::entry(
    &instance::pid,
    "pid",
    5),
  mapping::entry(
    &instance::program_end,
    "end_time",
    6),
  mapping::entry(
    &instance::program_start,
    "start_time",
    7),
  mapping::entry(
    &instance::version,
    "version",
    8),
  mapping::entry()
};

// Operations.
static io::data* new_instance() {
  return (new instance);
}
io::event_info::event_operations const instance::operations = {
  &new_instance
};
