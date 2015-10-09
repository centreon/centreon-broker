/*
** Copyright 2015 Centreon
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

#include <cmath>
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
host::host()
  : enable(true), host_id(0), poller_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
host::host(host const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
host::~host() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host& host::operator=(host const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are equal.
 */
bool host::operator==(host const& other) const {
  return (host_id == other.host_id
          && name == other.name);
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are not equal.
 */
bool host::operator!=(host const& other) const {
  return (!operator==(other));
}

/**
 *  Get object type.
 *
 *  @return Object type.
 */
unsigned int host::type() const {
  return (static_type());
}

/**
 *  Get class type.
 *
 *  @return Class type.
 */
unsigned int host::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_entries_host>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data objects.
 *
 *  @param[in] other  Object to copy.
 */
void host::_internal_copy(host const& other) {
  enable = other.enable;
  host_id = other.host_id;
  name = other.name;
  poller_id = other.poller_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const host::entries[] = {
  mapping::entry(
    &host::enable,
    "enable"),
  mapping::entry(
    &host::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &host::name,
    "name"),
  mapping::entry(
    &host::poller_id,
    "poller_id",
    mapping::entry::invalid_on_zero),
  mapping::entry()
};

// Operations.
static io::data* new_host() {
  return (new host);
}
io::event_info::event_operations const host::operations = {
  &new_host
};
