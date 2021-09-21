/*
** Copyright 2013, 2021 Centreon
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

/**
 * @brief The default constructor
 */
version_response::version_response()
    : io::data(version_response::static_type()),
      bbdo_major(BBDO_VERSION_MAJOR),
      bbdo_minor(BBDO_VERSION_MINOR),
      bbdo_patch(BBDO_VERSION_PATCH) {}

/**
 * @brief The constructor
 *
 * @param extensions A string with extensions asked for.
 */
version_response::version_response(std::string extensions)
    : io::data(version_response::static_type()),
      bbdo_major(BBDO_VERSION_MAJOR),
      bbdo_minor(BBDO_VERSION_MINOR),
      bbdo_patch(BBDO_VERSION_PATCH),
      extensions(std::move(extensions)) {}

// Mapping.
mapping::entry const version_response::entries[] = {
    mapping::entry(&version_response::bbdo_major, "major"),
    mapping::entry(&version_response::bbdo_minor, "minor"),
    mapping::entry(&version_response::bbdo_patch, "patch"),
    mapping::entry(&version_response::extensions, "extensions", 0),
    mapping::entry()};

// Operations.
static auto new_version_response() -> io::data* {
  return new version_response;
}
io::event_info::event_operations const version_response::operations = {
    &new_version_response};
