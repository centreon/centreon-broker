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
  : check_hosts_freshness(false),
    check_services_freshness(false),
    last_alive(0),
    last_command_check(0),
    obsess_over_hosts(false),
    obsess_over_services(false),
    poller_id(0) {}

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
  return (instance_status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int instance_status::static_type() {
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
  address = other.address;
  check_hosts_freshness = other.check_hosts_freshness;
  check_services_freshness = other.check_services_freshness;
  description = other.description;
  global_host_event_handler = other.global_host_event_handler;
  global_service_event_handler = other.global_service_event_handler;
  last_alive = other.last_alive;
  last_command_check = other.last_command_check;
  obsess_over_hosts = other.obsess_over_hosts;
  obsess_over_services = other.obsess_over_services;
  poller_id = other.poller_id;
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
    &instance_status::address,
    "address"),
  mapping::entry(
    &instance_status::check_hosts_freshness,
    "check_hosts_freshness"),
  mapping::entry(
    &instance_status::check_services_freshness,
    "check_services_freshness"),
  mapping::entry(
    &instance_status::description,
    "description"),
  mapping::entry(
    &instance_status::event_handler_enabled,
    "event_handlers"),
  mapping::entry(
    &instance_status::flap_detection_enabled,
    "flap_detection"),
  mapping::entry(
    &instance_status::poller_id,
    "instance_id",
    mapping::entry::invalid_on_zero,
    false),
  mapping::entry(
    &instance_status::last_alive,
    "last_alive"),
  mapping::entry(
    &instance_status::last_command_check,
    "last_command_check"),
  mapping::entry(
    &instance_status::obsess_over_hosts,
    "obsess_over_hosts"),
  mapping::entry(
    &instance_status::obsess_over_services,
    "obsess_over_services"),
  mapping::entry(
    &instance_status::global_host_event_handler,
    "global_host_event_handler"),
  mapping::entry(
    &instance_status::global_service_event_handler,
    "global_service_event_handler"),
  mapping::entry()
};

// Operations.
static io::data* new_is() {
  return (new instance_status);
}
io::event_info::event_operations const instance_status::operations = {
  &new_is
};
