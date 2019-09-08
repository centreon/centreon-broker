/*
** Copyright 2015 Centreon
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

#include "com/centreon/broker/correlation/log_issue.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/io/events.hh"

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
log_issue::log_issue() : host_id(0), service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
log_issue::log_issue(log_issue const& i) : io::data(i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
log_issue::~log_issue() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
log_issue& log_issue::operator=(log_issue const& i) {
  io::data::operator=(i);
  _internal_copy(i);
  return (*this);
}

/**
 *  Check that two issues are equal.
 *
 *  @param[in] i Issue to compare to.
 *
 *  @return true if both issues are equal.
 */
bool log_issue::operator==(log_issue const& i) const {
  return ((this == &i) ||
          ((log_ctime == i.log_ctime) && (host_id == i.host_id) &&
           (service_id == i.service_id) &&
           (issue_start_time == i.issue_start_time)));
}

/**
 *  Check that two issues are not equal.
 *
 *  @param[in] i Issue to compare to.
 *
 *  @return true if both issues are not equal.
 */
bool log_issue::operator!=(log_issue const& i) const {
  return (!this->operator==(i));
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int log_issue::type() const {
  return (log_issue::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int log_issue::static_type() {
  return (io::events::data_type<io::events::correlation,
                                correlation::de_log_issue>::value);
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
void log_issue::_internal_copy(log_issue const& i) {
  log_ctime = i.log_ctime;
  host_id = i.host_id;
  service_id = i.service_id;
  issue_start_time = i.issue_start_time;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const log_issue::entries[] = {
    mapping::entry(&log_issue::log_ctime,
                   "log_ctime",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&log_issue::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&log_issue::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&log_issue::issue_start_time,
                   "issue_start_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry()};

// Operations.
static io::data* new_log_issue() {
  return (new log_issue);
}
io::event_info::event_operations const log_issue::operations = {&new_log_issue};
