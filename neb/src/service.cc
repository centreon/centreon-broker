/*
** Copyright 2009-2013 Merethis
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
#include "com/centreon/broker/neb/service.hh"

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
 *  Initialize members to 0, NULL or equivalent.
 */
service::service() {
  _zero_initialize();
}

/**
 *  @brief Build a service from a service_status.
 *
 *  Copy all members from service_status to the current instance and
 *  zero-initialize remaining members.
 *
 *  @param[in] ss Object to copy.
 */
service::service(service_status const& ss) : service_status(ss) {
  _zero_initialize();
}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given service object to the current
 *  instance.
 *
 *  @param[in] s Object to copy.
 */
service::service(service const& s)
  : host_service(s), service_status(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
service::~service() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given service object to the current
 *  instance.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
service& service::operator=(service const& s) {
  host_service::operator=(s);
  service_status::operator=(s);
  _internal_copy(s);
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
unsigned int service::type() const {
  return (service::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int service::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_service>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy members of the service object to the current instance.
 *
 *  Copy all members defined within the service class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  @param[in] s Object to copy.
 */
void service::_internal_copy(service const& s) {
  failure_prediction_options = s.failure_prediction_options;
  flap_detection_on_critical = s.flap_detection_on_critical;
  flap_detection_on_ok = s.flap_detection_on_ok;
  flap_detection_on_unknown = s.flap_detection_on_unknown;
  flap_detection_on_warning = s.flap_detection_on_warning;
  is_volatile = s.is_volatile;
  notify_on_critical = s.notify_on_critical;
  notify_on_unknown = s.notify_on_unknown;
  notify_on_warning = s.notify_on_warning;
  stalk_on_critical = s.stalk_on_critical;
  stalk_on_ok = s.stalk_on_ok;
  stalk_on_unknown = s.stalk_on_unknown;
  stalk_on_warning = s.stalk_on_warning;
  return ;
}

/**
 *  @brief Initialize members to 0, NULL or equivalent.
 *
 *  This method initializes members defined within the Service class to
 *  0, NULL or equivalent. This method is used by some constructors.
 */
void service::_zero_initialize() {
  flap_detection_on_critical = false;
  flap_detection_on_ok = false;
  flap_detection_on_unknown = false;
  flap_detection_on_warning = false;
  is_volatile = false;
  notify_on_critical = false;
  notify_on_unknown = false;
  notify_on_warning = false;
  stalk_on_critical = false;
  stalk_on_ok = false;
  stalk_on_unknown = false;
  stalk_on_warning = false;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

//static_cast<double (host::*) >(&host::low_flap_threshold),

// Mapping. Some pointer-to-member are explicitely casted because they
// are from the host_service class which does not inherit from io::data.
mapping::entry const service::entries[] = {
  mapping::entry(
    &service::acknowledgement_type,
    "acknowledgement_type",
    1),
  mapping::entry(
    static_cast<QString (service::*)>(&service::action_url),
    "action_url",
    2),
  mapping::entry(
    &service::active_checks_enabled,
    "active_checks",
    3),
  mapping::entry(
    static_cast<bool (service::*)>(&service::check_freshness),
    "check_freshness",
    4),
  mapping::entry(
    &service::check_interval,
    "check_interval",
    5),
  mapping::entry(
    &service::check_period,
    "check_period",
    6),
  mapping::entry(
    &service::check_type,
    "check_type",
    7),
  mapping::entry(
    &service::current_check_attempt,
    "check_attempt",
    8),
  mapping::entry(
    &service::current_notification_number,
    "notification_number",
    9),
  mapping::entry(
    &service::current_state,
    "state",
    10),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_active_checks_enabled),
    "default_active_checks",
    0),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_event_handler_enabled),
    "default_event_handler_enabled",
    0),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_failure_prediction),
    "default_failure_prediction",
    0),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_flap_detection_enabled),
    "default_flap_detection",
    0),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_notifications_enabled),
    "default_notify",
    0),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_passive_checks_enabled),
    "default_passive_checks",
    0),
  mapping::entry(
    static_cast<bool (service::*)>(&service::default_process_perf_data),
    "default_process_perfdata",
    0),
  mapping::entry(
    static_cast<QString (service::*)>(&service::display_name),
    "display_name",
    11),
  mapping::entry(
    &service::enabled,
    "enabled",
    12),
  mapping::entry(
    &service::event_handler,
    "event_handler",
    13),
  mapping::entry(
    &service::event_handler_enabled,
    "event_handler_enabled",
    14),
  mapping::entry(
    &service::execution_time,
    "execution_time",
    15),
  mapping::entry(
    &service::failure_prediction_enabled,
    "failure_prediction",
    16),
  mapping::entry(
    &service::failure_prediction_options,
    "failure_prediction_options",
    17),
  mapping::entry(
    static_cast<double (service::*)>(&service::first_notification_delay),
    "first_notification_delay",
    18),
  mapping::entry(
    &service::flap_detection_enabled,
    "flap_detection",
    19),
  mapping::entry(
    &service::flap_detection_on_critical,
    "flap_detection_on_critical",
    20),
  mapping::entry(
    &service::flap_detection_on_ok,
    "flap_detection_on_ok",
    21),
  mapping::entry(
    &service::flap_detection_on_unknown,
    "flap_detection_on_unknown",
    22),
  mapping::entry(
    &service::flap_detection_on_warning,
    "flap_detection_on_warning",
    23),
  mapping::entry(
    static_cast<double (service::*)>(&service::freshness_threshold),
    "freshness_threshold",
    24),
  mapping::entry(
    &service::has_been_checked,
    "checked",
    25),
  mapping::entry(
    static_cast<double (service::*)>(&service::high_flap_threshold),
    "high_flap_threshold",
    26),
  mapping::entry(
    &service::host_id,
    "host_id",
    27,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service::host_name,
    "",
    28),
  mapping::entry(
    static_cast<QString (service::*)>(&service::icon_image),
    "icon_image",
    29),
  mapping::entry(
    static_cast<QString (service::*)>(&service::icon_image_alt),
    "icon_image_alt",
    30),
  mapping::entry(
    &service::service_id,
    "service_id",
    31,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service::is_flapping,
    "flapping",
    32),
  mapping::entry(
    &service::is_volatile,
    "volatile",
    33),
  mapping::entry(
    &service::last_check,
    "last_check",
    34),
  mapping::entry(
    &service::last_hard_state,
    "last_hard_state",
    35),
  mapping::entry(
    &service::last_hard_state_change,
    "last_hard_state_change",
    36),
  mapping::entry(
    &service::last_notification,
    "last_notification",
    37),
  mapping::entry(
    &service::last_state_change,
    "last_state_change",
    38),
  mapping::entry(
    &service::last_time_critical,
    "last_time_critical",
    39),
  mapping::entry(
    &service::last_time_ok,
    "last_time_ok",
    40),
  mapping::entry(
    &service::last_time_unknown,
    "last_time_unknown",
    41),
  mapping::entry(
    &service::last_time_warning,
    "last_time_warning",
    42),
  mapping::entry(
    &service::last_update,
    "last_update",
    43),
  mapping::entry(
    &service::latency,
    "latency",
    44),
  mapping::entry(
    static_cast<double (service::*)>(&service::low_flap_threshold),
    "low_flap_threshold",
    45),
  mapping::entry(
    &service::max_check_attempts,
    "max_check_attempts",
    46),
  mapping::entry(
    &service::modified_attributes,
    "modified_attributes",
    47),
  mapping::entry(
    &service::next_check,
    "next_check",
    48),
  mapping::entry(
    &service::next_notification,
    "next_notification",
    49),
  mapping::entry(
    &service::no_more_notifications,
    "no_more_notifications",
    50),
  mapping::entry(
    static_cast<QString (service::*)>(&service::notes),
    "notes",
    51),
  mapping::entry(
    static_cast<QString (service::*)>(&service::notes_url),
    "notes_url",
    52),
  mapping::entry(
    static_cast<double (service::*)>(&service::notification_interval),
    "notification_interval",
    53),
  mapping::entry(
    static_cast<QString (service::*)>(&service::notification_period),
    "notification_period",
    54),
  mapping::entry(
    &service::notifications_enabled,
    "notify",
    55),
  mapping::entry(
    &service::notify_on_critical,
    "notify_on_critical",
    56),
  mapping::entry(
    static_cast<bool (service::*)>(&service::notify_on_downtime),
    "notify_on_downtime",
    57),
  mapping::entry(
    static_cast<bool (service::*)>(&service::notify_on_flapping),
    "notify_on_flapping",
    58),
  mapping::entry(
    static_cast<bool (service::*)>(&service::notify_on_recovery),
    "notify_on_recovery",
    59),
  mapping::entry(
    &service::notify_on_unknown,
    "notify_on_unknown",
    60),
  mapping::entry(
    &service::notify_on_warning,
    "notify_on_warning",
    61),
  mapping::entry(
    &service::obsess_over,
    "obsess_over_service",
    62),
  mapping::entry(
    &service::passive_checks_enabled,
    "passive_checks",
    63),
  mapping::entry(
    &service::percent_state_change,
    "percent_state_change",
    64),
  mapping::entry(
    &service::problem_has_been_acknowledged,
    "acknowledged",
    65),
  mapping::entry(
    &service::process_performance_data,
    "process_perfdata",
    66),
  mapping::entry(
    static_cast<bool (service::*)>(&service::retain_nonstatus_information),
    "retain_nonstatus_information",
    67),
  mapping::entry(
    static_cast<bool (service::*)>(&service::retain_status_information),
    "retain_status_information",
    68),
  mapping::entry(
    &service::retry_interval,
    "retry_interval",
    69),
  mapping::entry(
    &service::scheduled_downtime_depth,
    "scheduled_downtime_depth",
    70),
  mapping::entry(
    &service::service_description,
    "description",
    71),
  mapping::entry(
    &service::should_be_scheduled,
    "should_be_scheduled",
    72),
  mapping::entry(
    &service::stalk_on_critical,
    "stalk_on_critical",
    73),
  mapping::entry(
    &service::stalk_on_ok,
    "stalk_on_ok",
    74),
  mapping::entry(
    &service::stalk_on_unknown,
    "stalk_on_unknown",
    75),
  mapping::entry(
    &service::stalk_on_warning,
    "stalk_on_warning",
    76),
  mapping::entry(
    &service::state_type,
    "state_type",
    77),
  mapping::entry(
    &service::check_command,
    "check_command",
    78),
  mapping::entry(
    &service::output,
    "output",
    79),
  mapping::entry(
    &service::perf_data,
    "perfdata",
    80),
  mapping::entry(
    &service::instance_id,
    "",
    81),
  mapping::entry()
};

// Operations.
static io::data* new_service() {
  return (new service);
}
io::event_info::event_operations const service::operations = {
  &new_service
};
