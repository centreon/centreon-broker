/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/bool_status.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_status::bool_status() : bool_id(0), state(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
bool_status::bool_status(bool_status const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
bool_status::~bool_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
bool_status& bool_status::operator=(bool_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int bool_status::type() const {
  return (bool_status::static_type());
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int bool_status::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_bool_status>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void bool_status::_internal_copy(bool_status const& other) {
  bool_id = other.bool_id;
  state = other.state;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const bool_status::entries[] = {
  mapping::entry(
    &bam::bool_status::bool_id,
    "bool_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &bam::bool_status::state,
    "state",
    2),
  mapping::entry(
    &bam::bool_status::state,
    "",
    3),
  mapping::entry()
};

// Operations.
static io::data* new_bool_status() {
  return (new bool_status);
}
io::event_info::event_operations const bool_status::operations = {
  &new_bool_status
};
