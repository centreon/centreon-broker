/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// /**
//  *  Internal protocol objects mappings.
//  */
// static mapped_data<version_response> const version_response_mapping[] = {
//   mapped_data<version_response>(
//     &version_response::bbdo_major,
//     1,
//     "major"),
//   mapped_data<version_response>(
//     &version_response::bbdo_minor,
//     2,
//     "minor"),
//   mapped_data<version_response>(
//     &version_response::bbdo_patch,
//     3,
//     "patch"),
//   mapped_data<version_response>(
//     &version_response::extensions,
//     4,
//     "extensions"),
//   mapped_data<version_response>()
// };

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
