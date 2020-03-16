/*
** Copyright 2009-2012.2015 Centreon
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

#include "com/centreon/broker/neb/group_member.hh"

using namespace com::centreon::broker;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
neb::group_member::group_member(uint32_t type)
    : io::data(type), enabled(true), group_id(0), host_id(0), poller_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
neb::group_member::group_member(group_member const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
neb::group_member::~group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
neb::group_member& neb::group_member::operator=(group_member const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  @brief Copy data members.
 *
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] other  Object to copy.
 */
void neb::group_member::_internal_copy(group_member const& other) {
  enabled = other.enabled;
  group_id = other.group_id;
  group_name = other.group_name;
  host_id = other.host_id;
  poller_id = other.poller_id;
  return;
}
