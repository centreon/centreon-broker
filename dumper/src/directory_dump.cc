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

#include "com/centreon/broker/dumper/directory_dump.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
directory_dump::directory_dump()
  : started(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
directory_dump::directory_dump(directory_dump const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
directory_dump::~directory_dump() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
directory_dump& directory_dump::operator=(directory_dump const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int directory_dump::type() const {
  return (static_type());
}

/**
 *  Get event class type.
 *
 *  @return Event class type.
 */
unsigned int directory_dump::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_directory_dump>::value);
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
void directory_dump::_internal_copy(directory_dump const& other) {
  started = other.started;
  tag = other.tag;
  req_id = other.req_id;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const directory_dump::entries[] = {
  mapping::entry(
    &directory_dump::tag,
    "tag"),
  mapping::entry(
    &directory_dump::started,
    "started"),
  mapping::entry(
    &directory_dump::req_id,
    "req_id"),
  mapping::entry()
};

// Operations.
static io::data* new_directory_dump() {
  return (new directory_dump);
}
io::event_info::event_operations const directory_dump::operations = {
  &new_directory_dump
};
