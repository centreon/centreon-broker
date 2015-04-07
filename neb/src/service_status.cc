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
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
service_status::service_status()
  : last_time_critical(0),
    last_time_ok(0),
    last_time_unknown(0),
    last_time_warning(0),
    service_id(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given service_status to the current
 *  instance.
 *
 *  @param[in] ss Object to copy.
 */
service_status::service_status(service_status const& ss)
  : host_service_status(ss) {
  _internal_copy(ss);
}

/**
 *  Destructor.
 */
service_status::~service_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given service_status to the current
 *  instance.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_status& service_status::operator=(service_status const& ss) {
  host_service_status::operator=(ss);
  _internal_copy(ss);
  return (*this);
}

/**
 *  Returns the type of the event.
 *
 *  @return The event_type.
 */
unsigned int service_status::type() const {
  return (service_status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int service_status::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_service_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members defined within service_status to
 *  the current instance. This method is used by the copy constructor
 *  and the assignment operator.
 *
 *  @param[in] ss Object to copy.
 */
void service_status::_internal_copy(service_status const& ss) {
  host_name = ss.host_name;
  last_time_critical = ss.last_time_critical;
  last_time_ok = ss.last_time_ok;
  last_time_unknown = ss.last_time_unknown;
  last_time_warning = ss.last_time_warning;
  service_description = ss.service_description;
  service_id = ss.service_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const service_status::entries[] = {
  mapping::entry(
    &service_status::active_checks_enabled,
    "active_checks",
    2),
  mapping::entry(
    &service_status::check_interval,
    "check_interval",
    3),
  mapping::entry(
    &service_status::check_period,
    "check_period",
    4),
  mapping::entry(
    &service_status::check_type,
    "check_type",
    5),
  mapping::entry(
    &service_status::current_check_attempt,
    "check_attempt",
    6),
  mapping::entry(
    &service_status::current_state,
    "state",
    8),
  mapping::entry(
    &service_status::enabled,
    "enabled",
    9),
  mapping::entry(
    &service_status::event_handler,
    "event_handler",
    10),
  mapping::entry(
    &service_status::event_handler_enabled,
    "event_handler_enabled",
    11),
  mapping::entry(
    &service_status::execution_time,
    "execution_time",
    12),
  mapping::entry(
    &service_status::flap_detection_enabled,
    "flap_detection",
    14),
  mapping::entry(
    &service_status::has_been_checked,
    "checked",
    15),
  mapping::entry(
    &service_status::host_id,
    "host_id",
    16,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_status::host_name,
    "",
    17),
  mapping::entry(
    &service_status::is_flapping,
    "flapping",
    18),
  mapping::entry(
    &service_status::last_check,
    "last_check",
    19),
  mapping::entry(
    &service_status::last_hard_state,
    "last_hard_state",
    20),
  mapping::entry(
    &service_status::last_hard_state_change,
    "last_hard_state_change",
    21),
  mapping::entry(
    &service_status::last_state_change,
    "last_state_change",
    23),
  mapping::entry(
    &service_status::last_time_critical,
    "last_time_critical",
    24),
  mapping::entry(
    &service_status::last_time_ok,
    "last_time_ok",
    25),
  mapping::entry(
    &service_status::last_time_unknown,
    "last_time_unknown",
    26),
  mapping::entry(
    &service_status::last_time_warning,
    "last_time_warning",
    27),
  mapping::entry(
    &service_status::last_update,
    "last_update",
    28),
  mapping::entry(
    &service_status::latency,
    "latency",
    29),
  mapping::entry(
    &service_status::max_check_attempts,
    "max_check_attempts",
    30),
  mapping::entry(
    &service_status::next_check,
    "next_check",
    32),
  mapping::entry(
    &service_status::obsess_over,
    "obsess_over_service",
    36),
  mapping::entry(
    &service_status::percent_state_change,
    "percent_state_change",
    38),
  mapping::entry(
    &service_status::retry_interval,
    "retry_interval",
    41),
  mapping::entry(
    &service_status::service_description,
    "",
    43),
  mapping::entry(
    &service_status::service_id,
    "service_id",
    44,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_status::should_be_scheduled,
    "should_be_scheduled",
    45),
  mapping::entry(
    &service_status::state_type,
    "state_type",
    46),
  mapping::entry(
    &service_status::check_command,
    "check_command",
    47),
  mapping::entry(
    &service_status::output,
    "output",
    48),
  mapping::entry(
    &service_status::perf_data,
    "perfdata",
    49),
  mapping::entry(
    &service_status::instance_id,
    "",
    50),
  mapping::entry()
};

// Operations.
static io::data* new_service_status() {
  return (new service_status);
}
io::event_info::event_operations const service_status::operations = {
  &new_service_status
};
