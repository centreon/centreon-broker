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

#include "com/centreon/broker/neb/service_dependency.hh"

#include "com/centreon/broker/database/table_max_size.hh"

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
service_dependency::service_dependency()
    : dependency(service_dependency::static_type()),
      dependent_service_id(0),
      service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] sd Object to copy.
 */
service_dependency::service_dependency(service_dependency const& sd)
    : dependency(sd) {
  _internal_copy(sd);
}

/**
 *  Destructor.
 */
service_dependency::~service_dependency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] sd Object to copy.
 *
 *  @return This object.
 */
service_dependency& service_dependency::operator=(
    service_dependency const& sd) {
  if (this != &sd) {
    dependency::operator=(sd);
    _internal_copy(sd);
  }
  return *this;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal members from the given object.
 *
 *  @param[in] sd Object to copy.
 */
void service_dependency::_internal_copy(service_dependency const& sd) {
  dependent_service_id = sd.dependent_service_id;
  service_id = sd.service_id;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const service_dependency::entries[] = {
    mapping::entry(&service_dependency::dependency_period,
                   "dependency_period",
                   get_services_services_dependencies_col_size(
                       services_services_dependencies_dependency_period)),
    mapping::entry(&service_dependency::dependent_host_id,
                   "dependent_host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&service_dependency::dependent_service_id,
                   "dependent_service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&service_dependency::enabled, ""),
    mapping::entry(
        &service_dependency::execution_failure_options,
        "execution_failure_options",
        get_services_services_dependencies_col_size(
            services_services_dependencies_execution_failure_options)),
    mapping::entry(&service_dependency::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&service_dependency::inherits_parent, "inherits_parent"),
    mapping::entry(
        &service_dependency::notification_failure_options,
        "notification_failure_options",
        get_services_services_dependencies_col_size(
            services_services_dependencies_notification_failure_options)),
    mapping::entry(&service_dependency::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_service_dependency() {
  return new service_dependency;
}
io::event_info::event_operations const service_dependency::operations = {
    &new_service_dependency};
