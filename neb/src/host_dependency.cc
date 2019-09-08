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

#include "com/centreon/broker/neb/host_dependency.hh"
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
 *  Default constructor.
 */
host_dependency::host_dependency() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
host_dependency::host_dependency(host_dependency const& other)
    : dependency(other) {}

/**
 *  Destructor.
 */
host_dependency::~host_dependency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_dependency& host_dependency::operator=(host_dependency const& other) {
  dependency::operator=(other);
  return (*this);
}

/**
 *  Get the type of this object.
 *
 *  @return The event type.
 */
unsigned int host_dependency::type() const {
  return (host_dependency::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int host_dependency::static_type() {
  return (
      io::events::data_type<io::events::neb, neb::de_host_dependency>::value);
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const host_dependency::entries[] = {
    mapping::entry(&host_dependency::dependency_period, "dependency_period"),
    mapping::entry(&host_dependency::dependent_host_id,
                   "dependent_host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_dependency::enabled, ""),
    mapping::entry(&host_dependency::execution_failure_options,
                   "execution_failure_options"),
    mapping::entry(&host_dependency::inherits_parent, "inherits_parent"),
    mapping::entry(&host_dependency::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_dependency::notification_failure_options,
                   nullptr,
                   mapping::entry::always_valid,
                   true,
                   "notification_failure_options"),
    mapping::entry()};

// Operations.
static io::data* new_host_dep() {
  return (new host_dependency);
}
io::event_info::event_operations const host_dependency::operations = {
    &new_host_dep};
