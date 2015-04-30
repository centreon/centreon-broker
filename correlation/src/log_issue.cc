/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/log_issue.hh"
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
log_issue::log_issue()
  : host_id(0),
    service_id(0) {}

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
  return ((this == &i)
          || ((log_ctime == i.log_ctime)
              && (host_id == i.host_id)
              && (service_id == i.service_id)
              && (issue_start_time == i.issue_start_time)));
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
  return (io::events::data_type<io::events::correlation, correlation::de_log_issue>::value);
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
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const log_issue::entries[] = {
  mapping::entry(
    &log_issue::log_ctime,
    "log_ctime",
    mapping::entry::invalid_on_minus_one),
  mapping::entry(
    &log_issue::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &log_issue::service_id,
    "service_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &log_issue::issue_start_time,
    "issue_start_time",
    mapping::entry::invalid_on_minus_one),
  mapping::entry()
};

// Operations.
static io::data* new_log_issue() {
  return (new log_issue);
}
io::event_info::event_operations const log_issue::operations = {
  &new_log_issue
};
