/*
** Copyright 2009-2013,2020 Centreon
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

#include "com/centreon/broker/correlation/issue.hh"

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
issue::issue()
    : io::data(issue::static_type()),
      ack_time(-1),
      end_time(-1),
      host_id(0),
      service_id(0),
      start_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
issue::issue(issue const& i) : io::data(i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
issue::~issue() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
issue& issue::operator=(issue const& i) {
  io::data::operator=(i);
  _internal_copy(i);
  return *this;
}

/**
 *  Check that two issues are equal.
 *
 *  @param[in] i Issue to compare to.
 *
 *  @return true if both issues are equal.
 */
bool issue::operator==(issue const& i) const {
  return this == &i || (ack_time == i.ack_time && end_time == i.end_time &&
                        host_id == i.host_id && service_id == i.service_id &&
                        start_time == i.start_time);
}

/**
 *  Check that two issues are not equal.
 *
 *  @param[in] i Issue to compare to.
 *
 *  @return true if both issues are not equal.
 */
bool issue::operator!=(issue const& i) const {
  return !this->operator==(i);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  @brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] i Object to copy.
 */
void issue::_internal_copy(issue const& i) {
  ack_time = i.ack_time;
  end_time = i.end_time;
  host_id = i.host_id;
  service_id = i.service_id;
  start_time = i.start_time;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const issue::entries[] = {
    mapping::entry(&issue::ack_time,
                   "ack_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&issue::end_time,
                   "end_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&issue::host_id, "host_id", mapping::entry::invalid_on_zero),
    mapping::entry(&issue::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&issue::start_time,
                   "start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry()};

// Operations.
static io::data* new_issue() {
  return new issue;
}
io::event_info::event_operations const issue::operations = {&new_issue};
