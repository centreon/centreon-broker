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

#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_ba_bv_relation_event::dimension_ba_bv_relation_event()
  : ba_id(0),
    bv_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_ba_bv_relation_event::dimension_ba_bv_relation_event(
    dimension_ba_bv_relation_event const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_ba_bv_relation_event::~dimension_ba_bv_relation_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_ba_bv_relation_event& dimension_ba_bv_relation_event::operator=(
    dimension_ba_bv_relation_event const& other) {
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
bool dimension_ba_bv_relation_event::operator==(
    dimension_ba_bv_relation_event const& other) const {
  return ((ba_id == other.ba_id)
          && (bv_id == other.bv_id));
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_ba_bv_relation_event::type() const {
  return (dimension_ba_bv_relation_event::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_ba_bv_relation_event::static_type() {
  return (io::events::data_type<io::events::bam,
                                bam::de_dimension_ba_bv_relation_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_ba_bv_relation_event::_internal_copy(
    dimension_ba_bv_relation_event const& other) {
  ba_id = other.ba_id;
  bv_id = other.bv_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dimension_ba_bv_relation_event::entries[] = {
  mapping::entry(
    &bam::dimension_ba_bv_relation_event::ba_id,
    "ba_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &bam::dimension_ba_bv_relation_event::bv_id,
    "bv_id",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &bam::dimension_ba_bv_relation_event::instance_id,
    "",
    3),
  mapping::entry()
};

// Operations.
static io::data* new_dimension_ba_bv_relation_event() {
  return (new dimension_ba_bv_relation_event);
}
io::event_info::event_operations const
  dimension_ba_bv_relation_event::operations = {
  &new_dimension_ba_bv_relation_event
};
