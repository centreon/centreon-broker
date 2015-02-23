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

#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_bv_event::dimension_bv_event()
  : bv_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_bv_event::dimension_bv_event(dimension_bv_event const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_bv_event::~dimension_bv_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_bv_event& dimension_bv_event::operator=(
    dimension_bv_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test for equality.
 *
 *  @return  True if the two objects are equal.
 */
bool dimension_bv_event::operator==(
    dimension_bv_event const& other) const {
  return ((bv_id == other.bv_id)
          && (bv_name == other.bv_name)
          && (bv_description == other.bv_description));
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_bv_event::type() const {
  return (dimension_bv_event::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_bv_event::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_dimension_bv_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_bv_event::_internal_copy(dimension_bv_event const& other) {
  bv_id = other.bv_id;
  bv_name = other.bv_name;
  bv_description = other.bv_description;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dimension_bv_event::entries[] = {
  mapping::entry(
    &bam::dimension_bv_event::bv_id,
    "bv_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &bam::dimension_bv_event::bv_name,
    "bv_name",
    2),
  mapping::entry(
    &bam::dimension_bv_event::bv_description,
    "bv_description",
    3),
  mapping::entry(
    &bam::dimension_bv_event::instance_id,
    "",
    4),
  mapping::entry()
};

// Operations.
static io::data* new_dimension_bv_event() {
  return (new dimension_bv_event);
}
io::event_info::event_operations const dimension_bv_event::operations = {
  &new_dimension_bv_event
};
