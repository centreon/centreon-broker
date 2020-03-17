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

#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
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
    : io::data(downtime::static_type()),
      actual_end_time(-1),
      actual_start_time(-1),
      deletion_time(-1),
      downtime_type(0),
      fixed(true),
      host_id(0),
      internal_id(0),
      poller_id(0),
      service_id(0),
      triggered_by(0),
      was_cancelled(false),
      was_started(false),
      is_recurring(false),
      come_from(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
downtime::downtime(downtime const& other) : io::data(other) {
  _internal_copy(other);
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
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
downtime& downtime::operator=(downtime const& other) {
  io::data::operator=(other);
  _internal_copy(other);
  return *this;
}

/**
 *  Comparison operator.
 *
 *  @param[in] o      The object to compare with.
 *
 *  @return           True if both objects are equal.
 */
bool downtime::operator==(downtime const& o) const {
  return ((actual_end_time == o.actual_end_time) &&
          (actual_start_time == o.actual_start_time) && (author == o.author) &&
          (comment == o.comment) && (deletion_time == o.deletion_time) &&
          (downtime_type == o.downtime_type) && (duration == o.duration) &&
          (end_time == o.end_time) && (entry_time == o.entry_time) &&
          (fixed == o.fixed) && (host_id == o.host_id) &&
          (internal_id == o.internal_id) && (poller_id == o.poller_id) &&
          (service_id == o.service_id) && (start_time == o.start_time) &&
          (triggered_by == o.triggered_by) &&
          (was_cancelled == o.was_cancelled) &&
          (was_started == o.was_started) && (is_recurring == o.is_recurring) &&
          (recurring_timeperiod == o.recurring_timeperiod) &&
          (come_from == o.come_from));
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
 *  @param[in] other  Object to copy.
 *
 *  @see downtime(downtime const&)
 *  @see operator=(downtime const&)
 */
void downtime::_internal_copy(downtime const& other) {
  actual_end_time = other.actual_end_time;
  actual_start_time = other.actual_start_time;
  author = other.author;
  comment = other.comment;
  deletion_time = other.deletion_time;
  downtime_type = other.downtime_type;
  duration = other.duration;
  end_time = other.end_time;
  entry_time = other.entry_time;
  fixed = other.fixed;
  host_id = other.host_id;
  internal_id = other.internal_id;
  poller_id = other.poller_id;
  service_id = other.service_id;
  start_time = other.start_time;
  triggered_by = other.triggered_by;
  was_cancelled = other.was_cancelled;
  was_started = other.was_started;
  is_recurring = other.is_recurring;
  recurring_timeperiod = other.recurring_timeperiod;
  come_from = other.come_from;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const downtime::entries[] = {
    mapping::entry(&downtime::actual_end_time,
                   "actual_end_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&downtime::actual_start_time,
                   "actual_start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&downtime::author, "author"),
    mapping::entry(&downtime::downtime_type, "type"),
    mapping::entry(&downtime::deletion_time,
                   "deletion_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&downtime::duration, "duration"),
    mapping::entry(&downtime::end_time,
                   "end_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&downtime::entry_time,
                   "entry_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&downtime::fixed, "fixed"),
    mapping::entry(&downtime::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&downtime::poller_id,
                   "instance_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&downtime::internal_id, "internal_id"),
    mapping::entry(&downtime::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&downtime::start_time,
                   "start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&downtime::triggered_by,
                   "triggered_by",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&downtime::was_cancelled, "cancelled"),
    mapping::entry(&downtime::was_started, "started"),
    mapping::entry(&downtime::comment, "comment_data"),
    mapping::entry(&downtime::is_recurring,
                   "is_recurring",
                   mapping::entry::invalid_on_v2),
    mapping::entry(&downtime::recurring_timeperiod,
                   "recurring_timeperiod",
                   mapping::entry::invalid_on_v2),
    mapping::entry(&downtime::come_from, ""),
    mapping::entry()};

// Operations.
static io::data* new_downtime() {
  return new downtime;
}
io::event_info::event_operations const downtime::operations = {&new_downtime};
