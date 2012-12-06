/*
** Copyright 2009-2012 Merethis
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
  : ack_time(0),
    end_time(0),
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
  return (*this);
}

/**
 *  Check that two issues are equal.
 *
 *  @param[in] i Issue to compare to.
 *
 *  @return true if both issues are equal.
 */
bool issue::operator==(issue const& i) const {
  return ((this == &i)
          || ((ack_time == i.ack_time)
              && (end_time == i.end_time)
              && (host_id == i.host_id)
              && (service_id == i.service_id)
              && (start_time == i.start_time)));
}

/**
 *  Check that two issues are not equal.
 *
 *  @param[in] i Issue to compare to.
 *
 *  @return true if both issues are not equal.
 */
bool issue::operator!=(issue const& i) const {
  return (!this->operator==(i));
}

/**
 *  Get the type of this event.
 *
 *  @return The string "com::centreon::broker::correlation::issue".
 */
QString const& issue::type() const {
  static QString const issue_type("com::centreon::broker::correlation::issue");
  return (issue_type);
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
  return ;
}
