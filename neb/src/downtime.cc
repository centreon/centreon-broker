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
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
downtime::downtime()
  : actual_end_time(0),
    actual_start_time(0),
    deletion_time(0),
    downtime_type(0),
    duration(0),
    end_time(0),
    entry_time(0),
    fixed(true),
    host_id(0),
        internal_id(0),
    service_id(0),
    start_time(0),
    triggered_by(0),
    was_cancelled(false),
    was_started(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  @param[in] d Object to copy.
 */
downtime::downtime(downtime const& d) : io::data(d) {
  _internal_copy(d);
}

/**
 *  Destructor.
 */
downtime::~downtime() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  @param[in] d Object to copy.
 *
 *  @return This object.
 */
downtime& downtime::operator=(downtime const& d) {
  io::data::operator=(d);
  _internal_copy(d);
  return (*this);
}

/**
 *  Get the event's type.
 *
 *  @return The event type.
 */
unsigned int downtime::type() const {
  return (io::events::data_type<io::events::neb, neb::de_downtime>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the given object to the current object.
 *
 *  This internal method is used to copy data defined inside the
 *  downtime class from an object to the current instance. This means
 *  that no superclass data are copied. This method is used in downtime
 *  copy constructor and in the assignment operator.
 *
 *  @param[in] d Object to copy.
 *
 *  @see downtime(downtime const&)
 *  @see operator=(downtime const&)
 */
void downtime::_internal_copy(downtime const& d) {
  actual_end_time = d.actual_end_time;
  actual_start_time = d.actual_start_time;
  author = d.author;
  comment = d.comment;
  deletion_time = d.deletion_time;
  downtime_type = d.downtime_type;
  duration = d.duration;
  end_time = d.end_time;
  entry_time = d.entry_time;
  fixed = d.fixed;
  host_id = d.host_id;
    internal_id = d.internal_id;
  service_id = d.service_id;
  start_time = d.start_time;
  triggered_by = d.triggered_by;
  was_cancelled = d.was_cancelled;
  was_started = d.was_started;
  return ;
}
