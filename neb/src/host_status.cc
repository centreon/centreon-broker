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
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  Initialize all members to 0, NULL or equivalent.
 */
host_status::host_status()
  : last_time_down(0),
    last_time_unreachable(0),
    last_time_up(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
host_status::host_status(host_status const& other)
  : host_service_status(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
host_status::~host_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_status& host_status::operator=(host_status const& other) {
  if (this != &other) {
    host_service_status::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Returns the type of the event.
 *
 *  @return The event_type.
 */
unsigned int host_status::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy all internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members of HostStatus to the current
 *  instance. This method is use by the copy constructor and the
 *  assignment operator.
 *
 *  @param[in] other  Object to copy.
 */
void host_status::_internal_copy(host_status const& other) {
  last_time_down = other.last_time_down;
  last_time_unreachable = other.last_time_unreachable;
  last_time_up = other.last_time_up;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const host_status::entries[] = {
  mapping::entry(
    &host_status::acknowledgement_type,
    "acknowledgement_type",
    1),
  mapping::entry(
    &host_status::active_checks_enabled,
    "active_checks",
    2),
  mapping::entry(
    &host_status::check_interval,
    "check_interval",
    3),
  mapping::entry(
    &host_status::check_period,
    "check_period",
    4),
  mapping::entry(
    &host_status::check_type,
    "check_type",
    5),
  mapping::entry(
    &host_status::current_check_attempt,
    "check_attempt",
    6),
  mapping::entry(
    &host_status::current_notification_number,
    "notification_number",
    7),
  mapping::entry(
    &host_status::current_state,
    "state",
    8),
  mapping::entry(
    &host_status::enabled,
    "enabled",
    9),
  mapping::entry(
    &host_status::event_handler,
    "event_handler",
    10),
  mapping::entry(
    &host_status::event_handler_enabled,
    "event_handler_enabled",
    11),
  mapping::entry(
    &host_status::execution_time,
    "execution_time",
    12),
  mapping::entry(
    &host_status::failure_prediction_enabled,
    "failure_prediction",
    13),
  mapping::entry(
    &host_status::flap_detection_enabled,
    "flap_detection",
    14),
  mapping::entry(
    &host_status::has_been_checked,
    "checked",
    15),
  mapping::entry(
    &host_status::host_id,
    "host_id",
    16,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host_status::is_flapping,
    "flapping",
    17),
  mapping::entry(
    &host_status::last_check,
    "last_check",
    18),
  mapping::entry(
    &host_status::last_hard_state,
    "last_hard_state",
    19),
  mapping::entry(
    &host_status::last_hard_state_change,
    "last_hard_state_change",
    20),
  mapping::entry(
    &host_status::last_notification,
    "last_notification",
    21),
  mapping::entry(
    &host_status::last_state_change,
    "last_state_change",
    22),
  mapping::entry(
    &host_status::last_time_down,
    "last_time_down",
    23),
  mapping::entry(
    &host_status::last_time_unreachable,
    "last_time_unreachable",
    24),
  mapping::entry(
    &host_status::last_time_up,
    "last_time_up",
    25),
  mapping::entry(
    &host_status::last_update,
    "last_update",
    26),
  mapping::entry(
    &host_status::latency,
    "latency",
    27),
  mapping::entry(
    &host_status::max_check_attempts,
    "max_check_attempts",
    28),
  mapping::entry(
    &host_status::modified_attributes,
    "modified_attributes",
    29),
  mapping::entry(
    &host_status::next_check,
    "next_check",
    30),
  mapping::entry(
    &host_status::next_notification,
    "next_host_notification",
    31),
  mapping::entry(
    &host_status::no_more_notifications,
    "no_more_notifications",
    32),
  mapping::entry(
    &host_status::notifications_enabled,
    "notify",
    33),
  mapping::entry(
    &host_status::obsess_over,
    "obsess_over_host",
    34),
  mapping::entry(
    &host_status::passive_checks_enabled,
    "passive_checks",
    35),
  mapping::entry(
    &host_status::percent_state_change,
    "percent_state_change",
    36),
  mapping::entry(
    &host_status::problem_has_been_acknowledged,
    "acknowledged",
    37),
  mapping::entry(
    &host_status::process_performance_data,
    "process_perfdata",
    38),
  mapping::entry(
    &host_status::retry_interval,
    "retry_interval",
    39),
  mapping::entry(
    &host_status::scheduled_downtime_depth,
    "scheduled_downtime_depth",
    40),
  mapping::entry(
    &host_status::should_be_scheduled,
    "should_be_scheduled",
    41),
  mapping::entry(
    &host_status::state_type,
    "state_type",
    42),
  mapping::entry(
    &host_status::check_command,
    "check_command",
    43),
  mapping::entry(
    &host_status::output,
    "output",
    44),
  mapping::entry(
    &host_status::perf_data,
    "perfdata",
    45),
  mapping::entry(
    &host_status::instance_id,
    "",
    46),
  mapping::entry()
};

// Operations.
static io::data* new_host_status() {
  return (new host_status);
}
io::event_info::event_operations const host_status::operations = {
  &new_host_status
};
