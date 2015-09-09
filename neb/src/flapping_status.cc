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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/flapping_status.hh"
#include "com/centreon/broker/neb/internal.hh"

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
 *  @param[in] fs Object to copy.
 */
flapping_status::flapping_status(flapping_status const& fs)
  : io::data(fs) {
  _internal_copy(fs);
}

/**
 *  Destructor.
 */
flapping_status::~flapping_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] fs Object to copy.
 *
 *  @return This object.
 */
flapping_status& flapping_status::operator=(flapping_status const& fs) {
  io::data::operator=(fs);
  _internal_copy(fs);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int flapping_status::type() const {
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
 *  @param[in] fs Object to copy.
 */
void flapping_status::_internal_copy(flapping_status const& fs) {
  comment_time = fs.comment_time;
  event_time = fs.event_time;
  event_type = fs.event_type;
  flapping_type = fs.flapping_type;
  high_threshold = fs.high_threshold;
  host_id = fs.host_id;
    internal_comment_id = fs.internal_comment_id;
  low_threshold = fs.low_threshold;
  percent_state_change = fs.percent_state_change;
  reason_type = fs.reason_type;
  service_id = fs.service_id;
  return ;
}
