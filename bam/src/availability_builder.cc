/*
** Copyright 2014-2015 Centreon
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

#include <ctime>
#include "com/centreon/broker/bam/availability_builder.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] ending_point    The point from when the builder should stop to build the availability.
 *  @param[in] starting_point  The point from when the builder should build the availability.
 */
availability_builder::availability_builder(
                        time_t ending_point,
                        time_t starting_point /* = 0 */)
  : _start(starting_point),
    _end(ending_point),
    _available(0),
    _unavailable(0),
    _degraded(0),
    _unknown(0),
    _downtime(0),
    _alert_unavailable_opened(0),
    _alert_degraded_opened(0),
    _alert_unknown_opened(0),
    _nb_downtime(0),
    _timeperiods_is_default(false) {}

/**
 *  Destructor
 */
availability_builder::~availability_builder() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
availability_builder::availability_builder(availability_builder const& other) {
  availability_builder::operator=(other);
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return  A reference to this object.
 */
availability_builder& availability_builder::operator=(availability_builder const& other) {
  if (this != &other) {
    _start = other._start;
    _end = other._end;
    _available = other._available;
    _unavailable = other._unavailable;
    _degraded = other._degraded;
    _unknown = other._unknown;
    _downtime = other._downtime;
    _alert_unavailable_opened = other._alert_unavailable_opened;
    _alert_degraded_opened = other._alert_degraded_opened;
    _alert_unknown_opened = other._alert_unknown_opened;
    _nb_downtime = other._nb_downtime;
    _timeperiods_is_default = other._timeperiods_is_default;
  }
  return (*this);
}

/**
 *  Add an event to the builder.
 *
 *  @param[in] status           The status of the event.
 *  @param[in] start            The start time of the event.
 *  @param[in] end              The end of the event.
 *  @param[in] was_in_downtime  Was the event in downtime?
 *  @param[in] tp               The timeperiod of the event.
 */
void availability_builder::add_event(
                             short status,
                             time_t start, time_t end,
                             bool was_in_downtime,
                             time::timeperiod::ptr const& tp) {
  // Check that the event was closed.
  if (end == 0)
    end = _end;
  // Check that the end of the event is not before the starting point of the
  // computing.
  if (end < _start)
    return ;
  // Check if event was opened "today".
  bool opened_today((start >= _start) && (start < _end));
  // Check that the event times are within the computed day.
  if (start < _start)
    start = _start;
  if (_end < end)
    end = _end;

  // Compute the sla_duration on the period.
  unsigned int sla_duration = tp->duration_intersect(start, end);
  if (sla_duration == (unsigned int)-1)
    return ;

  // Update the data.
  if (was_in_downtime) {
    _downtime += sla_duration;
    if (opened_today)
      ++_nb_downtime;
  }
  else {
    if (status == 0)
      _available += sla_duration;
    else if (status == 1) {
      _degraded += sla_duration;
      if (opened_today)
	++_alert_degraded_opened;
    }
    else if (status == 2) {
      _unavailable += sla_duration;
      if (opened_today)
	++_alert_unavailable_opened;
    }
    else {
      _unknown += sla_duration;
      if (opened_today)
	++_alert_unknown_opened;
    }
  }
}

/**
 *  Get the duration in second when the BA was ok.
 *
 *  @return  The duration in second when the BA was ok.
 */
int availability_builder::get_available() const {
  return (_available);
}

/**
 *  Get the duration in second when the BA was critical.
 *
 *  @return  The duration in second when the BA was critical.
 */
int availability_builder::get_unavailable() const {
  return (_unavailable);
}

/**
 *  Get the duration in second when the BA was in a warning state.
 *
 *  @return  The duration in second when the BA was in a warning state.
 */
int availability_builder::get_degraded() const {
  return (_degraded);
}

/**
 * Get the duration in second when the BA was in an unknown state.
 *
 * @return The duration in second when the BA was in an unknown state.
 */
int availability_builder::get_unknown() const {
  return (_unknown);
}

/**
 *  Get the duration in second when the BA was in downtime.
 *
 *  @return  The duration in second when the BA was in downtime.
 */
int availability_builder::get_downtime() const {
  return (_downtime);
}

/**
 *  Get the number of unavailable event opened.
 *
 *  @return  The number of unavailable event opened.
 */
int availability_builder::get_unavailable_opened() const {
  return (_alert_unavailable_opened);
}

/**
 *  Get the number of degraded event opened.
 *
 *  @return  The number of degraded event opened.
 */
int availability_builder::get_degraded_opened() const {
  return (_alert_degraded_opened);
}

/**
 *  Get the number of unknown event opened.
 *
 *  @return  The number of unknown event opened.
 */
int availability_builder::get_unknown_opened() const {
  return (_alert_unknown_opened);
}

/**
 *  Get the number of downtime event opened.
 *
 *  @return  The number of downtime event opened.
 */
int availability_builder::get_downtime_opened() const {
  return (_nb_downtime);
}

/**
 *  Set if the timeperiod is default.
 *
 *  @param[in] val  True if the timeperiod is default.
 */
void availability_builder::set_timeperiod_is_default(bool val) {
  _timeperiods_is_default = val;
}

/**
 *  Get if the timeperiod is default.
 *
 *  @param[in] val  True if the timeperiod is default.
 */
bool availability_builder::get_timeperiod_is_default() const {
  return (_timeperiods_is_default);
}
