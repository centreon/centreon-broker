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

#include "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_timeperiod_exception::dimension_timeperiod_exception()
  : timeperiod_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_timeperiod_exception::dimension_timeperiod_exception(
                                  dimension_timeperiod_exception const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_timeperiod_exception::~dimension_timeperiod_exception() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_timeperiod_exception& dimension_timeperiod_exception::operator=(
                                  dimension_timeperiod_exception const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_timeperiod_exception::type() const {
  return (dimension_timeperiod_exception::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_timeperiod_exception::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_dimension_timeperiod_exception>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void dimension_timeperiod_exception::_internal_copy(
                                       dimension_timeperiod_exception const& other) {
  daterange = other.daterange;
  timerange = other.timerange;
  timeperiod_id = other.timeperiod_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const dimension_timeperiod_exception::entries[] = {
  mapping::entry(
    &bam::dimension_timeperiod_exception::timeperiod_id,
    "timeperiod_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &bam::dimension_timeperiod_exception::daterange,
    "daterange"),
  mapping::entry(
    &bam::dimension_timeperiod_exception::timerange,
    "timerange"),
  mapping::entry()
};

// Operations.
static io::data* new_dimension_timeperiod_exception() {
  return (new dimension_timeperiod_exception);
}
io::event_info::event_operations const dimension_timeperiod_exception::operations = {
  &new_dimension_timeperiod_exception
};
