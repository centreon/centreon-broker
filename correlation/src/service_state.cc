/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_state::service_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
service_state::service_state(service_state const& ss) : state(ss) {}

/**
 *  Destructor.
 */
service_state::~service_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_state& service_state::operator=(service_state const& ss) {
  state::operator=(ss);
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] ss Object to compare to.
 *
 *  @return true if both objects are equal.
 */
bool service_state::operator==(service_state const& ss) const {
  return ((this == &ss)
          || ((ack_time == ss.ack_time)
              && (current_state == ss.current_state)
              && (end_time == ss.end_time)
              && (host_id == ss.host_id)
              && (in_downtime == ss.in_downtime)
              && (service_id == ss.service_id)
              && (start_time == ss.start_time)));
}

/**
 *  Non-equality operator.
 *
 *  @param[in] ss Object to compare to.
 *
 *  @return true if both objects are not equal.
 */
bool service_state::operator!=(service_state const& ss) const {
  return (!this->operator==(ss));
}

/**
 *  Get the type of this object.
 *
 *  @return The event type.
 */
unsigned int service_state::type() const {
  return (io::events::data_type<io::events::correlation, correlation::de_service_state>::value);
}
