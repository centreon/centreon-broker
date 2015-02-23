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
#include "com/centreon/broker/neb/flapping_status.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
flapping_status::flapping_status()
  : comment_time(0),
    event_time(0),
    event_type(0),
    flapping_type(0),
    high_threshold(0),
    host_id(0),
    internal_comment_id(0),
    low_threshold(0),
    percent_state_change(0),
    reason_type(0),
    service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
flapping_status::flapping_status(flapping_status const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
flapping_status::~flapping_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
flapping_status& flapping_status::operator=(
                                    flapping_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int flapping_status::type() const {
  return (flapping_status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int flapping_status::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_flapping_status>::value);
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
void flapping_status::_internal_copy(flapping_status const& other) {
  comment_time = other.comment_time;
  event_time = other.event_time;
  event_type = other.event_type;
  flapping_type = other.flapping_type;
  high_threshold = other.high_threshold;
  host_id = other.host_id;
  internal_comment_id = other.internal_comment_id;
  low_threshold = other.low_threshold;
  percent_state_change = other.percent_state_change;
  reason_type = other.reason_type;
  service_id = other.service_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const flapping_status::entries[] = {
  mapping::entry(
    &flapping_status::comment_time,
    "comment_time",
    1),
  mapping::entry(
    &flapping_status::event_time,
    "event_time",
    2),
  mapping::entry(
    &flapping_status::event_type,
    "event_type",
    3),
  mapping::entry(
    &flapping_status::flapping_type,
    "type",
    4),
  mapping::entry(
    &flapping_status::high_threshold,
    "high_threshold",
    5),
  mapping::entry(
    &flapping_status::host_id,
    "host_id",
    6,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &flapping_status::internal_comment_id,
    "internal_comment_id",
    7),
  mapping::entry(
    &flapping_status::low_threshold,
    "low_threshold",
    8),
  mapping::entry(
    &flapping_status::percent_state_change,
    "percent_state_change",
    9),
  mapping::entry(
    &flapping_status::reason_type,
    "reason_type",
    10),
  mapping::entry(
    &flapping_status::service_id,
    "service_id",
    11,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &flapping_status::instance_id,
    "",
    12),
  mapping::entry()
};

// Operations.
static io::data* new_flapping() {
  return (new flapping_status);
}
io::event_info::event_operations const flapping_status::operations = {
  &new_flapping
};
