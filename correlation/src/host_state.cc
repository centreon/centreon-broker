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
#include "com/centreon/broker/correlation/host_state.hh"
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
host_state::host_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hs Object to copy.
 */
host_state::host_state(host_state const& hs) : state(hs) {
  service_id = 0;
}

/**
 *  Destructor.
 */
host_state::~host_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] hs Object to copy.
 *
 *  @return This instance.
 */
host_state& host_state::operator=(host_state const& hs) {
  state::operator=(hs);
  service_id = 0;
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] hs Object to compare to.
 *
 *  @return true if both objects are equal.
 */
bool host_state::operator==(host_state const& hs) const {
  return ((this == &hs)
          || ((ack_time == hs.ack_time)
              && (current_state == hs.current_state)
              && (end_time == hs.end_time)
              && (host_id == hs.host_id)
              && (in_downtime == hs.in_downtime)
              && (start_time == hs.start_time)));
}

/**
 *  Non-equality operator.
 *
 *  @param[in] hs Object to compare to.
 *
 *  @return true if both objects are not equal.
 */
bool host_state::operator!=(host_state const& hs) const {
  return (!this->operator==(hs));
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host_state::type() const {
  return (io::events::data_type<io::events::correlation, correlation::de_host_state>::value);
}
