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
#include "com/centreon/broker/neb/instance_status.hh"
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
 *  Initialize members to 0, NULL or equivalent.
 */
instance_status::instance_status()
  : active_host_checks_enabled(false),
    active_service_checks_enabled(false),
    check_hosts_freshness(false),
    check_services_freshness(false),
    daemon_mode(false),
    id(0),
    last_alive(0),
    last_command_check(0),
    last_log_rotation(0),
    modified_host_attributes(0),
    modified_service_attributes(0),
    obsess_over_hosts(false),
    obsess_over_services(false),
    passive_host_checks_enabled(false),
    passive_service_checks_enabled(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] other  Object to copy data from.
 */
instance_status::instance_status(instance_status const& other)
  : status(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
instance_status::~instance_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] other Object to copy.
 */
instance_status& instance_status::operator=(
                                    instance_status const& other) {
  if (this != &other) {
    status::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
unsigned int instance_status::type() const {
  return (io::events::data_type<io::events::neb, neb::de_instance_status>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the instance_status object to the
 *         current instance.
 *
 *  Copy data defined within the instance_status class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  @param[in] other  Object to copy.
 */
void instance_status::_internal_copy(instance_status const& other) {
  active_host_checks_enabled = other.active_host_checks_enabled;
  active_service_checks_enabled = other.active_service_checks_enabled;
  address = other.address;
  check_hosts_freshness = other.check_hosts_freshness;
  check_services_freshness = other.check_services_freshness;
  daemon_mode = other.daemon_mode;
  description = other.description;
  global_host_event_handler = other.global_host_event_handler;
  global_service_event_handler = other.global_service_event_handler;
  id = other.id;
  last_alive = other.last_alive;
  last_command_check = other.last_command_check;
  last_log_rotation = other.last_log_rotation;
  modified_host_attributes = other.modified_host_attributes;
  modified_service_attributes = other.modified_service_attributes;
  obsess_over_hosts = other.obsess_over_hosts;
  obsess_over_services = other.obsess_over_services;
  passive_host_checks_enabled = other.passive_host_checks_enabled;
  passive_service_checks_enabled = other.passive_service_checks_enabled;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const instance_status::entries[] = {
  mapping::entry(
    &instance_status::active_host_checks_enabled,
    "active_host_checks",
    1),
  mapping::entry(
    &instance_status::active_service_checks_enabled,
    "active_service_checks",
    2),
  mapping::entry(
    &instance_status::address,
    "address",
    3),
  mapping::entry(
    &instance_status::check_hosts_freshness,
    "check_hosts_freshness",
    4),
  mapping::entry(
    &instance_status::check_services_freshness,
    "check_services_freshness",
    5),
  mapping::entry(
    &instance_status::daemon_mode,
    "daemon_mode",
    6),
  mapping::entry(
    &instance_status::description,
    "description",
    7),
  mapping::entry(
    &instance_status::event_handler_enabled,
    "event_handlers",
    8),
  mapping::entry(
    &instance_status::failure_prediction_enabled,
    "failure_prediction",
    9),
  mapping::entry(
    &instance_status::flap_detection_enabled,
    "flap_detection",
    10),
  mapping::entry(
    &instance_status::id,
    "instance_id",
    11,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &instance_status::last_alive,
    "last_alive",
    12),
  mapping::entry(
    &instance_status::last_command_check,
    "last_command_check",
    13),
  mapping::entry(
    &instance_status::last_log_rotation,
    "last_log_rotation",
    14),
  mapping::entry(
    &instance_status::modified_host_attributes,
    "modified_host_attributes",
    15),
  mapping::entry(
    &instance_status::modified_service_attributes,
    "modified_service_attributes",
    16),
  mapping::entry(
    &instance_status::notifications_enabled,
    "notifications",
    17),
  mapping::entry(
    &instance_status::obsess_over_hosts,
    "obsess_over_hosts",
    18),
  mapping::entry(
    &instance_status::obsess_over_services,
    "obsess_over_services",
    19),
  mapping::entry(
    &instance_status::passive_host_checks_enabled,
    "passive_host_checks",
    20),
  mapping::entry(
    &instance_status::passive_service_checks_enabled,
    "passive_service_checks",
    21),
  mapping::entry(
    &instance_status::process_performance_data,
    "process_perfdata",
    22),
  mapping::entry(
    &instance_status::global_host_event_handler,
    "global_host_event_handler",
    23),
  mapping::entry(
    &instance_status::global_service_event_handler,
    "global_service_event_handler",
    24),
  mapping::entry()
};

// Operations.
static io::data* new_is() {
  return (new instance_status);
}
io::event_info::event_operations const instance_status::operations = {
  &new_is
};
