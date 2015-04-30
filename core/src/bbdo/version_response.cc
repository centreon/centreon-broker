/*
** Copyright 2013,2015 Merethis
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
version_response& version_response::operator=(
                                      version_response const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int version_response::type() const {
  return (version_response::static_type());
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
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const version_response::entries[] = {
  mapping::entry(
    &version_response::bbdo_major,
    "major"),
  mapping::entry(
    &version_response::bbdo_minor,
    "minor"),
  mapping::entry(
    &version_response::bbdo_patch,
    "patch"),
  mapping::entry(
    &version_response::extensions,
    "extensions"),
  mapping::entry()
};

// Operations.
static io::data* new_version_response() {
  return (new version_response);
}
io::event_info::event_operations const version_response::operations = {
  &new_version_response
};
