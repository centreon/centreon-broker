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

#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_event::kpi_event()
  : kpi_id(0),
    impact_level(0),
    in_downtime(false),
    status(3) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
kpi_event::kpi_event(kpi_event const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
kpi_event::~kpi_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
kpi_event& kpi_event::operator=(kpi_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test.
 *
 *  @return  True if the two objects are equal.
 */
bool kpi_event::operator==(kpi_event const& other) const {
  return ((end_time == other.end_time)
          && (kpi_id == other.kpi_id)
          && (impact_level == other.impact_level)
          && (in_downtime == other.in_downtime)
          && (output == other.output)
          && (perfdata == other.perfdata)
          && (start_time == other.start_time)
          && (status == other.status));
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int kpi_event::type() const {
  return (kpi_event::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int kpi_event::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_kpi_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void kpi_event::_internal_copy(kpi_event const& other) {
  end_time = other.end_time;
  kpi_id = other.kpi_id;
  impact_level = other.impact_level;
  in_downtime = other.in_downtime;
  output = other.output;
  perfdata = other.perfdata;
  start_time = other.start_time;
  status = other.status;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const kpi_event::entries[] = {
  mapping::entry(
    &bam::kpi_event::kpi_id,
    "kpi_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &bam::kpi_event::end_time,
    "end_time",
    2),
  mapping::entry(
    &bam::kpi_event::impact_level,
    "impact_level",
    3),
  mapping::entry(
    &bam::kpi_event::in_downtime,
    "in_downtime",
    4),
  mapping::entry(
    &bam::kpi_event::output,
    "first_output",
    5),
  mapping::entry(
    &bam::kpi_event::perfdata,
    "first_perfdata",
    6),
  mapping::entry(
    &bam::kpi_event::start_time,
    "start_time",
    7),
  mapping::entry(
    &bam::kpi_event::status,
    "status",
    8),
  mapping::entry(
    &bam::kpi_event::instance_id,
    "",
    9),
  mapping::entry()
};

// Operations.
static io::data* new_kpi_event() {
  return (new kpi_event);
}
io::event_info::event_operations const kpi_event::operations = {
  &new_kpi_event
};
