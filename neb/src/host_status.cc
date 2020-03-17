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

#include "com/centreon/broker/neb/host_status.hh"

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
host_status::host_status(uint32_t type)
    : host_service_status(type),
      last_time_down(0),
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
  return *this;
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
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const host_status::entries[] = {
    mapping::entry(&host_status::acknowledged,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "acknowledged"),
    mapping::entry(&host_status::acknowledgement_type,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "acknowledgement_type"),
    mapping::entry(&host_status::active_checks_enabled, "active_checks"),
    mapping::entry(&host_status::check_interval, "check_interval"),
    mapping::entry(&host_status::check_period, "check_period"),
    mapping::entry(&host_status::check_type, "check_type"),
    mapping::entry(&host_status::current_check_attempt, "check_attempt"),
    mapping::entry(&host_status::current_state, "state"),
    mapping::entry(&host_status::downtime_depth,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "scheduled_downtime_depth"),
    mapping::entry(&host_status::enabled, "enabled"),
    mapping::entry(&host_status::event_handler, "event_handler"),
    mapping::entry(&host_status::event_handler_enabled,
                   "event_handler_enabled"),
    mapping::entry(&host_status::execution_time, "execution_time"),
    mapping::entry(&host_status::flap_detection_enabled, "flap_detection"),
    mapping::entry(&host_status::has_been_checked, "checked"),
    mapping::entry(&host_status::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::is_flapping, "flapping"),
    mapping::entry(&host_status::last_check,
                   "last_check",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::last_hard_state, "last_hard_state"),
    mapping::entry(&host_status::last_hard_state_change,
                   "last_hard_state_change",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::last_notification,
                   nullptr,
                   mapping::entry::invalid_on_zero,
                   true,
                   "last_notification"),
    mapping::entry(&host_status::last_state_change,
                   "last_state_change",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::last_time_down,
                   "last_time_down",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::last_time_unreachable,
                   "last_time_unreachable",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::last_time_up,
                   "last_time_up",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::last_update,
                   "last_update",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::latency, "latency"),
    mapping::entry(&host_status::max_check_attempts, "max_check_attempts"),
    mapping::entry(&host_status::next_check,
                   "next_check",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_status::next_notification,
                   nullptr,
                   mapping::entry::invalid_on_zero,
                   false,
                   "next_host_notification"),
    mapping::entry(&host_status::no_more_notifications,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "no_more_notifications"),
    mapping::entry(&host_status::notification_number,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "notification_number"),
    mapping::entry(&host_status::notifications_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "notify"),
    mapping::entry(&host_status::obsess_over, "obsess_over_host"),
    mapping::entry(&host_status::passive_checks_enabled,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "passive_checks"),
    mapping::entry(&host_status::percent_state_change, "percent_state_change"),
    mapping::entry(&host_status::retry_interval, "retry_interval"),
    mapping::entry(&host_status::should_be_scheduled, "should_be_scheduled"),
    mapping::entry(&host_status::state_type, "state_type"),
    mapping::entry(&host_status::check_command, "check_command"),
    mapping::entry(&host_status::output, "output"),
    mapping::entry(&host_status::perf_data, "perfdata"),
    mapping::entry()};

// Operations.
static io::data* new_host_status() {
  return new host_status;
}
io::event_info::event_operations const host_status::operations = {
    &new_host_status};
