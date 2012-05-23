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

#include "com/centreon/broker/neb/dependency.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
dependency::dependency()
  : dependent_host_id(0),
    enabled(true),
    host_id(0),
    inherits_parent(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] dep Object to copy.
 */
dependency::dependency(dependency const& dep) : io::data(dep) {
  _internal_copy(dep);
}

/**
 *  Destructor.
 */
dependency::~dependency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] dep Object to copy from.
 *
 *  @return This object.
 */
dependency& dependency::operator=(dependency const& dep) {
  io::data::operator=(dep);
  _internal_copy(dep);
  return (*this);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal data members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  @param[in] dep Object to copy.
 */
void dependency::_internal_copy(dependency const& dep) {
  dependency_period = dep.dependency_period;
  dependent_host_id = dep.dependent_host_id;
  enabled = dep.enabled;
  execution_failure_options = dep.execution_failure_options;
  host_id = dep.host_id;
  inherits_parent = dep.inherits_parent;
  notification_failure_options = dep.notification_failure_options;
  return ;
}
