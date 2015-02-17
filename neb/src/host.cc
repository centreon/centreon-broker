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
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/internal.hh"

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
 *  Initialize internal data to 0, NULL or equivalent.
 */
host::host() {
  _zero_initialize();
}

/**
 *  @brief Build a host from a host_status.
 *
 *  Copy host_status data to the current instance and zero-initialize
 *  other members.
 *
 *  @param[in] hs  host_status object to initialize part of the host
 *                 instance.
 */
host::host(host_status const& hs) : host_status(hs) {
  _zero_initialize();
}

/**
 *  @brief Copy constructor.
 *
 *  Copy data from the given object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
host::host(host const& other)
  : host_service(other),
    host_status(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
host::~host() {}

/**
 *  @brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host& host::operator=(host const& other) {
  if (this != &other) {
    host_service::operator=(other);
    host_status::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host::type() const {
  return (host::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int host::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_host>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy all internal data of the given object to the current
 *         instance.
 *
 *  This method copy all data defined directly in the host class. This
 *  is used by the copy constructor and the assignment operator.
 *
 *  @param[in] other  Object to copy data.
 */
void host::_internal_copy(host const& other) {
  address = other.address;
  alias = other.alias;
  flap_detection_on_down = other.flap_detection_on_down;
  flap_detection_on_unreachable = other.flap_detection_on_unreachable;
  flap_detection_on_up = other.flap_detection_on_up;
  host_name = other.host_name;
  notify_on_down = other.notify_on_down;
  notify_on_unreachable = other.notify_on_unreachable;
  stalk_on_down = other.stalk_on_down;
  stalk_on_unreachable = other.stalk_on_unreachable;
  stalk_on_up = other.stalk_on_up;
  statusmap_image = other.statusmap_image;
  return ;
}

/**
 *  @brief Zero-initialize internal data.
 *
 *  This method is used by constructors.
 */
void host::_zero_initialize() {
  flap_detection_on_down = 0;
  flap_detection_on_unreachable = 0;
  flap_detection_on_up = 0;
  instance_id = 0;
  notify_on_down = 0;
  notify_on_unreachable = 0;
  stalk_on_down = 0;
  stalk_on_unreachable = 0;
  stalk_on_up = 0;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping. Some pointer-to-member are explicitely casted because they
// are from the host_service class which does not inherit from io::data.
mapping::entry const host::entries[] = {
  mapping::entry(
    &host::acknowledgement_type,
    "acknowledgement_type",
    1),
  mapping::entry(
    static_cast<QString (host::*) >(&host::action_url),
    "action_url",
    2),
  mapping::entry(
    &host::active_checks_enabled,
    "active_checks",
    3),
  mapping::entry(
    &host::address,
    "address",
    4),
  mapping::entry(
    &host::alias,
    "alias",
    5),
  mapping::entry(
    static_cast<bool (host::*) >(&host::check_freshness),
    "check_freshness",
    6),
  mapping::entry(
    &host::check_interval,
    "check_interval",
    7),
  mapping::entry(
    &host::check_period,
    "check_period",
    8),
  mapping::entry(
    &host::check_type,
    "check_type",
    9),
  mapping::entry(
    &host::current_check_attempt,
    "check_attempt",
    10),
  mapping::entry(
    &host::current_notification_number,
    "notification_number",
    11),
  mapping::entry(
    &host::current_state,
    "state",
    12),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_active_checks_enabled),
    "default_active_checks",
    0),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_event_handler_enabled),
    "default_event_handler_enabled",
    0),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_failure_prediction),
    "default_failure_prediction",
    0),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_flap_detection_enabled),
    "default_flap_detection",
    0),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_notifications_enabled),
    "default_notify",
    0),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_passive_checks_enabled),
    "default_passive_checks",
    0),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_process_perf_data),
    "default_process_perfdata",
    0),
  mapping::entry(
    static_cast<QString (host::*) >(&host::display_name),
    "display_name",
    13),
  mapping::entry(
    &host::enabled,
    "enabled",
    14),
  mapping::entry(
    &host::event_handler,
    "event_handler",
    15),
  mapping::entry(
    &host::event_handler_enabled,
    "event_handler_enabled",
    16),
  mapping::entry(
    &host::execution_time,
    "execution_time",
    17),
  mapping::entry(
    &host::failure_prediction_enabled,
    "failure_prediction",
    18),
  mapping::entry(
    static_cast<double (host::*) >(&host::first_notification_delay),
    "first_notification_delay",
    19),
  mapping::entry(
    &host::flap_detection_enabled,
    "flap_detection",
    20),
  mapping::entry(
    &host::flap_detection_on_down,
    "flap_detection_on_down",
    21),
  mapping::entry(
    &host::flap_detection_on_unreachable,
    "flap_detection_on_unreachable",
    22),
  mapping::entry(
    &host::flap_detection_on_up,
    "flap_detection_on_up",
    23),
  mapping::entry(
    static_cast<double (host::*) >(&host::freshness_threshold),
    "freshness_threshold",
    24),
  mapping::entry(
    &host::has_been_checked,
    "checked",
    25),
  mapping::entry(
    static_cast<double (host::*) >(&host::high_flap_threshold),
    "high_flap_threshold",
    26),
  mapping::entry(
    &host::host_name,
    "name",
    27),
  mapping::entry(
    static_cast<QString (host::*) >(&host::icon_image),
    "icon_image",
    28),
  mapping::entry(
    static_cast<QString (host::*) >(&host::icon_image_alt),
    "icon_image_alt",
    29),
  mapping::entry(
    &host::host_id,
    "host_id",
    30,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host::instance_id,
    "instance_id",
    31,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host::is_flapping,
    "flapping",
    32),
  mapping::entry(
    &host::last_check,
    "last_check",
    33),
  mapping::entry(
    &host::last_hard_state,
    "last_hard_state",
    34),
  mapping::entry(
    &host::last_hard_state_change,
    "last_hard_state_change",
    35),
  mapping::entry(
    &host::last_notification,
    "last_notification",
    36),
  mapping::entry(
    &host::last_state_change,
    "last_state_change",
    37),
  mapping::entry(
    &host::last_time_down,
    "last_time_down",
    38),
  mapping::entry(
    &host::last_time_unreachable,
    "last_time_unreachable",
    39),
  mapping::entry(
    &host::last_time_up,
    "last_time_up",
    40),
  mapping::entry(
    &host::last_update,
    "last_update",
    41),
  mapping::entry(
    &host::latency,
    "latency",
    42),
  mapping::entry(
    static_cast<double (host::*) >(&host::low_flap_threshold),
    "low_flap_threshold",
    43),
  mapping::entry(
    &host::max_check_attempts,
    "max_check_attempts",
    44),
  mapping::entry(
    &host::modified_attributes,
    "modified_attributes",
    45),
  mapping::entry(
    &host::next_check,
    "next_check",
    46),
  mapping::entry(
    &host::next_notification,
    "next_host_notification",
    47),
  mapping::entry(
    &host::no_more_notifications,
    "no_more_notifications",
    48),
  mapping::entry(
    static_cast<QString (host::*) >(&host::notes),
    "notes",
    49),
  mapping::entry(
    static_cast<QString (host::*) >(&host::notes_url),
    "notes_url",
    50),
  mapping::entry(
    static_cast<double (host::*) >(&host::notification_interval),
    "notification_interval",
    51),
  mapping::entry(
    static_cast<QString (host::*) >(&host::notification_period),
    "notification_period",
    52),
  mapping::entry(
    &host::notifications_enabled,
    "notify",
    53),
  mapping::entry(
    &host::notify_on_down,
    "notify_on_down",
    54),
  mapping::entry(
    static_cast<bool (host::*) >(&host::notify_on_downtime),
    "notify_on_downtime",
    55),
  mapping::entry(
    static_cast<bool (host::*) >(&host::notify_on_flapping),
    "notify_on_flapping",
    56),
  mapping::entry(
    static_cast<bool (host::*) >(&host::notify_on_recovery),
    "notify_on_recovery",
    57),
  mapping::entry(
    &host::notify_on_unreachable,
    "notify_on_unreachable",
    58),
  mapping::entry(
    &host::obsess_over,
    "obsess_over_host",
    59),
  mapping::entry(
    &host::passive_checks_enabled,
    "passive_checks",
    60),
  mapping::entry(
    &host::percent_state_change,
    "percent_state_change",
    61),
  mapping::entry(
    &host::problem_has_been_acknowledged,
    "acknowledged",
    62),
  mapping::entry(
    &host::process_performance_data,
    "process_perfdata",
    63),
  mapping::entry(
    static_cast<bool (host::*) >(&host::retain_nonstatus_information),
    "retain_nonstatus_information",
    64),
  mapping::entry(
    static_cast<bool (host::*) >(&host::retain_status_information),
    "retain_status_information",
    65),
  mapping::entry(
    &host::retry_interval,
    "retry_interval",
    66),
  mapping::entry(
    &host::scheduled_downtime_depth,
    "scheduled_downtime_depth",
    67),
  mapping::entry(
    &host::should_be_scheduled,
    "should_be_scheduled",
    68),
  mapping::entry(
    &host::stalk_on_down,
    "stalk_on_down",
    69),
  mapping::entry(
    &host::stalk_on_unreachable,
    "stalk_on_unreachable",
    70),
  mapping::entry(
    &host::stalk_on_up,
    "stalk_on_up",
    71),
  mapping::entry(
    &host::state_type,
    "state_type",
    72),
  mapping::entry(
    &host::statusmap_image,
    "statusmap_image",
    73),
  mapping::entry(
    &host::check_command,
    "check_command",
    74),
  mapping::entry(
    &host::output,
    "output",
    75),
  mapping::entry(
    &host::perf_data,
    "perfdata",
    76),
  mapping::entry()
};

// Operations.
static io::data* new_host() {
  return (new host);
}
io::event_info::event_operations const host::operations = {
  &new_host
};
