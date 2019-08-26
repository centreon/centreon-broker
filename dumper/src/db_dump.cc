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

#include "com/centreon/broker/dumper/db_dump.hh"
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
db_dump::db_dump() : commit(false), full(false), poller_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
db_dump::db_dump(db_dump const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
db_dump::~db_dump() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
db_dump& db_dump::operator=(db_dump const& other) {
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
unsigned int db_dump::type() const {
  return (static_type());
}

/**
 *  Get event class type.
 *
 *  @return Event class type.
 */
unsigned int db_dump::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_db_dump>::value);
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
void db_dump::_internal_copy(db_dump const& other) {
  commit = other.commit;
  full = other.full;
  poller_id = other.poller_id;
  req_id = other.req_id;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const db_dump::entries[] = {
  mapping::entry(
    &db_dump::commit,
    "commit"),
  mapping::entry(
    &db_dump::full,
    "full"),
  mapping::entry(
    &db_dump::poller_id,
    "poller_id"),
  mapping::entry(
    &db_dump::req_id,
    "req_id"),
  mapping::entry()
};

// Operations.
static io::data* new_db_dump() {
  return (new db_dump);
}
io::event_info::event_operations const db_dump::operations = {
  &new_db_dump
};
