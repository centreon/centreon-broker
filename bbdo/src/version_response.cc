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

#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/io/events.hh"

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
  : bbdo_major(BBDO_VERSION_MAJOR),
    bbdo_minor(BBDO_VERSION_MINOR),
    bbdo_patch(BBDO_VERSION_PATCH) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
version_response::version_response(version_response const& right)
  : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
version_response::~version_response() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
version_response& version_response::operator=(
                                      version_response const& right) {
  if (this != &right) {
    io::data::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int version_response::type() const {
  return (io::events::data_type<io::events::bbdo, bbdo::de_version_response>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void version_response::_internal_copy(version_response const& right) {
  bbdo_major = right.bbdo_major;
  bbdo_minor = right.bbdo_minor;
  bbdo_patch = right.bbdo_patch;
  extensions = right.extensions;
  return ;
}
