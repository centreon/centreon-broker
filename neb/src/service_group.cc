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

#include "com/centreon/broker/neb/service_group.hh"

#include "com/centreon/broker/database/table_max_size.hh"

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
 *  Set all members to their default value (0, NULL or equivalent).
 */
service_group::service_group() : group(service_group::static_type()) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the service group object to the current
 *  instance.
 *
 *  @param[in] other  Object to copy.
 */
service_group::service_group(service_group const& other) : group(other) {}

/**
 *  Destructor.
 */
service_group::~service_group() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the service group object to the current
 *  instance.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
service_group& service_group::operator=(service_group const& other) {
  group::operator=(other);
  return *this;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const service_group::entries[] = {
    mapping::entry(&service_group::id,
                   "servicegroup_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&service_group::name,
                   "name",
                   get_servicegroups_col_size(servicegroups_name)),
    mapping::entry(&service_group::enabled, nullptr),
    mapping::entry(&service_group::poller_id,
                   nullptr,
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_service_group() {
  return new service_group;
}
io::event_info::event_operations const service_group::operations = {
    &new_service_group};
