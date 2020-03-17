/*
** Copyright 2009-2012,2015 Centreon
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
dependency::dependency(uint32_t type)
    : io::data(type),
      dependent_host_id(0),
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
  return;
}
