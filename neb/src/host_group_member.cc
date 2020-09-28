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

#include "com/centreon/broker/neb/host_group_member.hh"

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
host_group_member::host_group_member()
    : group_member(host_group_member::static_type()) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
host_group_member::host_group_member(host_group_member const& other)
    : group_member(other) {}

/**
 *  Destructor.
 */
host_group_member::~host_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_group_member& host_group_member::operator=(
    host_group_member const& other) {
  group_member::operator=(other);
  return *this;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const host_group_member::entries[] = {
    mapping::entry(&host_group_member::group_id,
                   "hostgroup_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_group_member::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_group_member::enabled, nullptr),
    mapping::entry(&host_group_member::group_name, nullptr, 0),
    mapping::entry(&host_group_member::poller_id,
                   nullptr,
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_host_group_member() {
  return new host_group_member;
}
io::event_info::event_operations const host_group_member::operations = {
    &new_host_group_member};
