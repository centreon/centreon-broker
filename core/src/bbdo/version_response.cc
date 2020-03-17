/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/bbdo/version_response.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
version_response::version_response()
    : io::data(version_response::static_type()),
      bbdo_major(BBDO_VERSION_MAJOR),
      bbdo_minor(BBDO_VERSION_MINOR),
      bbdo_patch(BBDO_VERSION_PATCH) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
version_response::version_response(version_response const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
version_response::~version_response() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
version_response& version_response::operator=(version_response const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return *this;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void version_response::_internal_copy(version_response const& other) {
  bbdo_major = other.bbdo_major;
  bbdo_minor = other.bbdo_minor;
  bbdo_patch = other.bbdo_patch;
  extensions = other.extensions;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const version_response::entries[] = {
    mapping::entry(&version_response::bbdo_major, "major"),
    mapping::entry(&version_response::bbdo_minor, "minor"),
    mapping::entry(&version_response::bbdo_patch, "patch"),
    mapping::entry(&version_response::extensions, "extensions"),
    mapping::entry()};

// Operations.
static io::data* new_version_response() {
  return new version_response;
}
io::event_info::event_operations const version_response::operations = {
    &new_version_response};
