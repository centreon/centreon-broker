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

#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_status::kpi_status()
  : kpi_id(0),
    level_acknowledgement_hard(0.0),
    level_acknowledgement_soft(0.0),
    level_downtime_hard(0.0),
    level_downtime_soft(0.0),
    level_nominal_hard(100.0),
    level_nominal_soft(100.0),
    state_hard(0),
    state_soft(0),
    last_impact(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
kpi_status::kpi_status(kpi_status const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
kpi_status::~kpi_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
kpi_status& kpi_status::operator=(kpi_status const& other) {
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
unsigned int kpi_status::type() const {
  return (kpi_status::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int kpi_status::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_kpi_status>::value);
}
/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void kpi_status::_internal_copy(kpi_status const& other) {
  kpi_id = other.kpi_id;
  level_acknowledgement_hard = other.level_acknowledgement_hard;
  level_acknowledgement_soft = other.level_acknowledgement_soft;
  level_downtime_hard = other.level_downtime_hard;
  level_downtime_soft = other.level_downtime_soft;
  level_nominal_hard = other.level_nominal_hard;
  level_nominal_soft = other.level_nominal_soft;
  state_hard = other.state_hard;
  state_soft = other.state_soft;
  last_state_change = other.last_state_change;
  last_impact = other.last_impact;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const kpi_status::entries[] = {
  mapping::entry(
    &bam::kpi_status::kpi_id,
    "kpi_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &bam::kpi_status::level_acknowledgement_hard,
    "level_acknowledgement_hard"),
  mapping::entry(
    &bam::kpi_status::level_acknowledgement_soft,
    "level_acknowledgement_soft"),
  mapping::entry(
    &bam::kpi_status::level_downtime_hard,
    "level_downtime_hard"),
  mapping::entry(
    &bam::kpi_status::level_downtime_soft,
    "level_downtime_soft"),
  mapping::entry(
    &bam::kpi_status::level_nominal_hard,
    "level_nominal_hard"),
  mapping::entry(
    &bam::kpi_status::level_nominal_soft,
    "level_nominal_soft"),
  mapping::entry(
    &bam::kpi_status::state_hard,
    "state_hard"),
  mapping::entry(
    &bam::kpi_status::state_soft,
    "state_soft"),
  mapping::entry(
    &bam::kpi_status::last_state_change,
    "last_state_change"),
  mapping::entry(
    &bam::kpi_status::last_impact,
    "last_impact"),
  mapping::entry()
};

// Operations.
static io::data* new_kpi_status() {
  return (new kpi_status);
}
io::event_info::event_operations const kpi_status::operations = {
  &new_kpi_status
};
