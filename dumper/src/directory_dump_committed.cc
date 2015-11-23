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

#include "com/centreon/broker/dumper/directory_dump_committed.hh"
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
directory_dump_committed::directory_dump_committed()
  : success(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
directory_dump_committed::directory_dump_committed(directory_dump_committed const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
directory_dump_committed::~directory_dump_committed() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
directory_dump_committed& directory_dump_committed::operator=(
  directory_dump_committed const& other) {
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
unsigned int directory_dump_committed::type() const {
  return (static_type());
}

/**
 *  Get event class type.
 *
 *  @return Event class type.
 */
unsigned int directory_dump_committed::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_directory_dump_committed>::value);
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
void directory_dump_committed::_internal_copy(
       directory_dump_committed const& other) {
  success = other.success;
  req_id = other.req_id;
  error_message = other.error_message;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const directory_dump_committed::entries[] = {
  mapping::entry(
    &directory_dump_committed::success,
    "success"),
  mapping::entry(
    &directory_dump_committed::req_id,
    "req_id"),
  mapping::entry(
    &directory_dump_committed::error_message,
    "error_message"),
  mapping::entry()
};

// Operations.
static io::data* new_directory_dump_committed() {
  return (new directory_dump_committed);
}
io::event_info::event_operations const directory_dump_committed::operations = {
  &new_directory_dump_committed
};
