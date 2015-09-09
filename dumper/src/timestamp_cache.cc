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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/dumper/timestamp_cache.hh"
#include "com/centreon/broker/dumper/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Default constructor.
 */
timestamp_cache::timestamp_cache() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
timestamp_cache::timestamp_cache(timestamp_cache const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
timestamp_cache::~timestamp_cache() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
timestamp_cache& timestamp_cache::operator=(timestamp_cache const& right) {
  if (this != &right) {
    io::data::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int timestamp_cache::type() const {
  return (timestamp_cache::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int timestamp_cache::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_timestamp_cache>::value);
}


/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void timestamp_cache::_internal_copy(timestamp_cache const& right) {
  filename = right.filename;
  last_modified = right.last_modified;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const timestamp_cache::entries[] = {
  mapping::entry(
    &timestamp_cache::filename,
    "filename"),
  mapping::entry(
    &timestamp_cache::last_modified,
    "last_modified",
    mapping::entry::invalid_on_minus_one),
  mapping::entry()
};

// Operations.
static io::data* new_timestamp_cache() {
  return (new timestamp_cache);
}
io::event_info::event_operations const timestamp_cache::operations = {
  &new_timestamp_cache
};
