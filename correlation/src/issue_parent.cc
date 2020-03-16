/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/correlation/issue_parent.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 */
issue_parent::issue_parent()
    : io::data(issue_parent::static_type()),
      child_host_id(0),
      child_service_id(0),
      child_start_time(0),
      end_time(-1),
      parent_host_id(0),
      parent_service_id(0),
      parent_start_time(0),
      start_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] issue_parent Object to copy.
 */
issue_parent::issue_parent(issue_parent const& ip) : io::data(ip) {
  _internal_copy(ip);
}

/**
 *  Destructor.
 */
issue_parent::~issue_parent() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ip Object to copy.
 *
 *  @return This object.
 */
issue_parent& issue_parent::operator=(issue_parent const& ip) {
  io::data::operator=(ip);
  _internal_copy(ip);
  return *this;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members to this object.
 *
 *  @param[in] ip Object to copy.
 */
void issue_parent::_internal_copy(issue_parent const& ip) {
  child_host_id = ip.child_host_id;
  child_service_id = ip.child_service_id;
  child_start_time = ip.child_start_time;
  end_time = ip.end_time;
  parent_host_id = ip.parent_host_id;
  parent_service_id = ip.parent_service_id;
  parent_start_time = ip.parent_start_time;
  start_time = ip.start_time;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const issue_parent::entries[] = {
    mapping::entry(&issue_parent::child_host_id,
                   "child_host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&issue_parent::child_service_id,
                   "child_service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&issue_parent::child_start_time,
                   "child_start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&issue_parent::end_time,
                   "end_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&issue_parent::parent_host_id,
                   "parent_host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&issue_parent::parent_service_id,
                   "parent_service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&issue_parent::parent_start_time,
                   "parent_start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&issue_parent::start_time,
                   "start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry()};

// Operations.
static io::data* new_issue_parent() {
  return new issue_parent;
}
io::event_info::event_operations const issue_parent::operations = {
    &new_issue_parent};
