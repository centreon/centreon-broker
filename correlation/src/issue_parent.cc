/*
** Copyright 2009-2011 Merethis
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

#include "com/centreon/broker/correlation/issue_parent.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

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
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
issue_parent::issue_parent()
  : child_host_id(0),
    child_service_id(0),
    child_start_time(0),
    end_time(0),
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
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The string "com::centreon::broker::correlation::issue_parent".
 */
QString const& issue_parent::type() const {
  static QString const ip_type("com::centreon::broker::correlation::issue_parent");
  return (ip_type);
}
