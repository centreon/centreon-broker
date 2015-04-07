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
  flap_detection_on_critical = s.flap_detection_on_critical;
  flap_detection_on_ok = s.flap_detection_on_ok;
  flap_detection_on_unknown = s.flap_detection_on_unknown;
  flap_detection_on_warning = s.flap_detection_on_warning;
  is_volatile = s.is_volatile;
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
    static_cast<bool (service::*)>(&service::default_flap_detection_enabled),
    "default_flap_detection",
    0),
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
    &service::next_check,
    "next_check",
    48),
  mapping::entry(
    &service::obsess_over,
    "obsess_over_service",
    62),
  mapping::entry(
    &service::percent_state_change,
    "percent_state_change",
    64),
  mapping::entry(
    &service::retry_interval,
    "retry_interval",
    69),
  mapping::entry(
    &service::service_description,
    "description",
    71),
  mapping::entry(
    &service::should_be_scheduled,
    "should_be_scheduled",
    72),
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
