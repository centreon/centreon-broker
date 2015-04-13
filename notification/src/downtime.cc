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
#include "com/centreon/broker/notification/downtime.hh"
#include "com/centreon/broker/notification/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

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
  return (*this);
}

/**
 *  Get the event's type.
 *
 *  @return The event type.
 */
unsigned int downtime::type() const {
  return (downtime::static_type());
}

/**
 *  Comparison operator.
 *
 *  @param[in] o      The object to compare with.
 *
 *  @return           True if both objects are equal.
 */
bool downtime::operator==(downtime const& o) const {
  return ((actual_end_time == o.actual_end_time)
          && (actual_start_time == o.actual_start_time)
          && (author == o.author)
          && (comment == o.comment)
          && (deletion_time == o.deletion_time)
          && (downtime_type == o.downtime_type)
          && (duration == o.duration)
          && (end_time == o.end_time)
          && (entry_time == o.entry_time)
          && (fixed == o.fixed)
          && (host_id == o.host_id)
          && (internal_id == o.internal_id)
          && (service_id == o.service_id)
          && (start_time == o.start_time)
          && (triggered_by == o.triggered_by)
          && (was_cancelled == o.was_cancelled)
          && (was_started == o.was_started));
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
  service_id = other.service_id;
  start_time = other.start_time;
  triggered_by = other.triggered_by;
  was_cancelled = other.was_cancelled;
  was_started = other.was_started;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const downtime::entries[] = {
  mapping::entry(
    &downtime::actual_end_time,
    "actual_end_time",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::actual_start_time,
    "actual_start_time",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::author,
    "author",
    3),
  mapping::entry(
    &downtime::downtime_type,
    "type",
    4),
  mapping::entry(
    &downtime::deletion_time,
    "deletion_time",
    5,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::duration,
    "duration",
    6),
  mapping::entry(
    &downtime::end_time,
    "end_time",
    7),
  mapping::entry(
    &downtime::entry_time,
    "entry_time",
    8),
  mapping::entry(
    &downtime::fixed,
    "fixed",
    9),
  mapping::entry(
    &downtime::host_id,
    "host_id",
    10,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::instance_id,
    "instance_id",
    11,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::internal_id,
    "internal_id",
    12),
  mapping::entry(
    &downtime::service_id,
    "service_id",
    13,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::start_time,
    "start_time",
    14),
  mapping::entry(
    &downtime::triggered_by,
    "triggered_by",
    15,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &downtime::was_cancelled,
    "cancelled",
    16),
  mapping::entry(
    &downtime::was_started,
    "started",
    17),
  mapping::entry(
    &downtime::comment,
    "comment_data",
    18),
  mapping::entry()
};

// Operations.
static io::data* new_downtime() {
  return (new downtime);
}
io::event_info::event_operations const downtime::operations = {
  &new_downtime
};
