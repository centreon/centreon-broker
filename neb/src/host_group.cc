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

#include "com/centreon/broker/neb/host_group.hh"
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
 *  Set all members to their default value (0, NULL or equivalent).
 */
host_group::host_group() {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the host group object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
host_group::host_group(host_group const& other) : group(other) {}

/**
 *  Destructor.
 */
host_group::~host_group() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the host group object to the current instance.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_group& host_group::operator=(host_group const& other) {
  group::operator=(other);
  return (*this);
}

/**
 *  Get this object type.
 *
 *  @return This object type.
 */
unsigned int host_group::type() const {
  return (static_type());
}

/**
 *  Get the class type.
 *
 *  @return Class type.
 */
unsigned int host_group::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_host_group>::value);
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const host_group::entries[] = {
    mapping::entry(&host_group::id,
                   "hostgroup_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_group::name, "name"),
    mapping::entry(&host_group::enabled, nullptr),
    mapping::entry(&host_group::poller_id,
                   nullptr,
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_host_group() {
  return (new host_group);
}
io::event_info::event_operations const host_group::operations = {
    &new_host_group};
