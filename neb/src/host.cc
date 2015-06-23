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
  poller_id = other.poller_id;
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
  poller_id = 0;
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
    &host::active_checks_enabled,
    "active_checks"),
  mapping::entry(
    &host::address,
    "address"),
  mapping::entry(
    &host::alias,
    "alias"),
  mapping::entry(
    static_cast<bool (host::*) >(&host::check_freshness),
    "check_freshness"),
  mapping::entry(
    &host::check_interval,
    "check_interval"),
  mapping::entry(
    &host::check_period,
    "check_period"),
  mapping::entry(
    &host::check_type,
    "check_type"),
  mapping::entry(
    &host::current_check_attempt,
    "check_attempt"),
  mapping::entry(
    &host::current_state,
    "state"),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_active_checks_enabled),
    "default_active_checks",
    mapping::entry::always_valid,
    false),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_event_handler_enabled),
    "default_event_handler_enabled",
    mapping::entry::always_valid,
    false),
  mapping::entry(
    static_cast<bool (host::*) >(&host::default_flap_detection_enabled),
    "default_flap_detection",
    mapping::entry::always_valid,
    false),
  mapping::entry(
    &host::enabled,
    "enabled"),
  mapping::entry(
    &host::event_handler,
    "event_handler"),
  mapping::entry(
    &host::event_handler_enabled,
    "event_handler_enabled"),
  mapping::entry(
    &host::execution_time,
    "execution_time"),
  mapping::entry(
    &host::flap_detection_enabled,
    "flap_detection"),
  mapping::entry(
    &host::flap_detection_on_down,
    "flap_detection_on_down"),
  mapping::entry(
    &host::flap_detection_on_unreachable,
    "flap_detection_on_unreachable"),
  mapping::entry(
    &host::flap_detection_on_up,
    "flap_detection_on_up"),
  mapping::entry(
    static_cast<double (host::*) >(&host::freshness_threshold),
    "freshness_threshold"),
  mapping::entry(
    &host::has_been_checked,
    "checked"),
  mapping::entry(
    static_cast<double (host::*) >(&host::high_flap_threshold),
    "high_flap_threshold"),
  mapping::entry(
    &host::host_name,
    "name"),
  mapping::entry(
    &host::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &host::poller_id,
    "instance_id",
    mapping::entry::invalid_on_zero,
    false),
  mapping::entry(
    &host::is_flapping,
    "flapping"),
  mapping::entry(
    &host::last_check,
    "last_check"),
  mapping::entry(
    &host::last_hard_state,
    "last_hard_state"),
  mapping::entry(
    &host::last_hard_state_change,
    "last_hard_state_change"),
  mapping::entry(
    &host::last_state_change,
    "last_state_change"),
  mapping::entry(
    &host::last_time_down,
    "last_time_down"),
  mapping::entry(
    &host::last_time_unreachable,
    "last_time_unreachable"),
  mapping::entry(
    &host::last_time_up,
    "last_time_up"),
  mapping::entry(
    &host::last_update,
    "last_update"),
  mapping::entry(
    &host::latency,
    "latency"),
  mapping::entry(
    static_cast<double (host::*) >(&host::low_flap_threshold),
    "low_flap_threshold"),
  mapping::entry(
    &host::max_check_attempts,
    "max_check_attempts"),
  mapping::entry(
    &host::next_check,
    "next_check"),
  mapping::entry(
    &host::obsess_over,
    "obsess_over_host"),
  mapping::entry(
    &host::percent_state_change,
    "percent_state_change"),
  mapping::entry(
    &host::retry_interval,
    "retry_interval"),
  mapping::entry(
    &host::should_be_scheduled,
    "should_be_scheduled"),
  mapping::entry(
    &host::state_type,
    "state_type"),
  mapping::entry(
    &host::check_command,
    "check_command"),
  mapping::entry(
    &host::output,
    "output"),
  mapping::entry(
    &host::perf_data,
    "perfdata"),
  mapping::entry()
};

// Operations.
static io::data* new_host() {
  return (new host);
}
io::event_info::event_operations const host::operations = {
  &new_host
};
