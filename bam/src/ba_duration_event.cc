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

#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
ba_duration_event::ba_duration_event()
  : ba_id(0),
    duration(0),
    sla_duration(0),
    timeperiod_id(0),
    timeperiod_is_default(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba_duration_event::ba_duration_event(ba_duration_event const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ba_duration_event::~ba_duration_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ba_duration_event& ba_duration_event::operator=(
    ba_duration_event const& other) {
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
bool ba_duration_event::operator==(ba_duration_event const& other) const {
  return ((ba_id == other.ba_id)
          && (end_time == other.end_time)
          && (start_time == other.start_time)
          && (duration == other.duration)
          && (sla_duration == other.sla_duration)
          && (timeperiod_id == other.timeperiod_id)
          && (timeperiod_is_default == other.timeperiod_is_default));
}


/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int ba_duration_event::type() const {
  return (io::events::data_type<io::events::bam, bam::de_ba_duration_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void ba_duration_event::_internal_copy(ba_duration_event const& other) {
  ba_id = other.ba_id;
  end_time = other.end_time;
  start_time = other.start_time;
  duration = other.duration;
  sla_duration = other.sla_duration;
  timeperiod_id = other.timeperiod_id;
  timeperiod_is_default = other.timeperiod_is_default;
  return ;
}
