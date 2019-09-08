/*
** Copyright 2009-2013,2015 Centreon
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

#include "com/centreon/broker/neb/instance_status.hh"
#include "com/centreon/broker/io/events.hh"
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
      last_alive((time_t)-1),
      last_command_check((time_t)-1),
      obsess_over_hosts(false),
      obsess_over_services(false),
      passive_host_checks_enabled(false),
      passive_service_checks_enabled(false),
      poller_id(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] other  Object to copy data from.
 */
instance_status::instance_status(instance_status const& other) : status(other) {
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
instance_status& instance_status::operator=(instance_status const& other) {
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
  return (
      io::events::data_type<io::events::neb, neb::de_instance_status>::value);
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
  check_hosts_freshness = other.check_hosts_freshness;
  check_services_freshness = other.check_services_freshness;
  global_host_event_handler = other.global_host_event_handler;
  global_service_event_handler = other.global_service_event_handler;
  last_alive = other.last_alive;
  last_command_check = other.last_command_check;
  obsess_over_hosts = other.obsess_over_hosts;
  obsess_over_services = other.obsess_over_services;
  passive_host_checks_enabled = other.passive_host_checks_enabled;
  passive_service_checks_enabled = other.passive_service_checks_enabled;
  poller_id = other.poller_id;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const instance_status::entries[] = {
    mapping::entry(&instance_status::active_host_checks_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "active_host_checks"),
    mapping::entry(&instance_status::active_service_checks_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "active_service_checks"),
    mapping::entry(&instance_status::check_hosts_freshness,
                   "check_hosts_freshness"),
    mapping::entry(&instance_status::check_services_freshness,
                   "check_services_freshness"),
    mapping::entry(&instance_status::event_handler_enabled, "event_handlers"),
    mapping::entry(&instance_status::flap_detection_enabled, "flap_detection"),
    mapping::entry(&instance_status::poller_id,
                   "instance_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&instance_status::last_alive,
                   "last_alive",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&instance_status::last_command_check,
                   "last_command_check",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&instance_status::notifications_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "notifications"),
    mapping::entry(&instance_status::obsess_over_hosts, "obsess_over_hosts"),
    mapping::entry(&instance_status::obsess_over_services,
                   "obsess_over_services"),
    mapping::entry(&instance_status::passive_host_checks_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "passive_host_checks"),
    mapping::entry(&instance_status::passive_service_checks_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "passive_service_checks"),
    mapping::entry(&instance_status::global_host_event_handler,
                   "global_host_event_handler"),
    mapping::entry(&instance_status::global_service_event_handler,
                   "global_service_event_handler"),
    mapping::entry()};

// Operations.
static io::data* new_is() {
  return (new instance_status);
}
io::event_info::event_operations const instance_status::operations = {&new_is};
