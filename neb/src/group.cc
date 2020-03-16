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

#include "com/centreon/broker/neb/group.hh"

using namespace com::centreon::broker::neb;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  @brief Default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
group::group(uint32_t type)
    : io::data(type), enabled(true), id(0), poller_id(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the Group object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
group::group(group const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
group::~group() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the group object to the current instance.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
group& group::operator=(group const& other) {
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
 *  @brief Copy internal members of the given object to the current
 *         instance.
 *
 *  This internal method is used to copy data defined inside the group
 *  class from an object to the current instance. This means that no
 *  superclass data are copied. This method is used in the copy
 *  constructor and in the assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @see group(group const&)
 *  @see operator=(group const&)
 */
void group::_internal_copy(group const& other) {
  enabled = other.enabled;
  id = other.id;
  name = other.name;
  poller_id = other.poller_id;
  return;
}
