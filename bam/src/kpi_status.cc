/*
** Copyright 2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
    last_impact(0),
    valid(true) {}

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
  valid = other.valid;
  return ;
}
