/*
** Copyright 2014-2015 Merethis
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

#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_ba_timeperiod_relation::dimension_ba_timeperiod_relation()
  : ba_id(0),
    timeperiod_id(0),
    is_default(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_ba_timeperiod_relation::dimension_ba_timeperiod_relation(
    dimension_ba_timeperiod_relation const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_ba_timeperiod_relation::~dimension_ba_timeperiod_relation() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_ba_timeperiod_relation& dimension_ba_timeperiod_relation::operator=(
    dimension_ba_timeperiod_relation const& other) {
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
bool dimension_ba_timeperiod_relation::operator==(
    dimension_ba_timeperiod_relation const& other) const {
  return ((ba_id == other.ba_id)
          && (timeperiod_id == other.timeperiod_id)
          && (is_default == other.is_default));
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_ba_timeperiod_relation::type() const {
  return (dimension_ba_timeperiod_relation::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_ba_timeperiod_relation::static_type() {
  return (io::events::data_type<io::events::bam,
                                bam::de_dimension_ba_timeperiod_relation>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_ba_timeperiod_relation::_internal_copy(
    dimension_ba_timeperiod_relation const& other) {
  ba_id = other.ba_id;
  timeperiod_id = other.timeperiod_id;
  is_default = other.is_default;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dimension_ba_timeperiod_relation::entries[] = {
  mapping::entry(
    &bam::dimension_ba_timeperiod_relation::ba_id,
    "ba_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &bam::dimension_ba_timeperiod_relation::timeperiod_id,
    "timeperiod_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &bam::dimension_ba_timeperiod_relation::is_default,
    "is_default"),
  mapping::entry()
};

// Operations.
static io::data* new_dimension_ba_timeperiod_relation() {
  return (new dimension_ba_timeperiod_relation);
}
io::event_info::event_operations const dimension_ba_timeperiod_relation::operations = {
  &new_dimension_ba_timeperiod_relation
};
