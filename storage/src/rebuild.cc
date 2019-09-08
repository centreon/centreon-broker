/*
** Copyright 2012-2013 Centreon
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

#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
rebuild::rebuild() : end(true), id(0), is_index(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
rebuild::rebuild(rebuild const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
rebuild::~rebuild() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
rebuild& rebuild::operator=(rebuild const& right) {
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
unsigned int rebuild::type() const {
  return (rebuild::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int rebuild::static_type() {
  return (
      io::events::data_type<io::events::storage, storage::de_rebuild>::value);
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
void rebuild::_internal_copy(rebuild const& right) {
  end = right.end;
  id = right.id;
  is_index = right.is_index;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const rebuild::entries[] = {
    mapping::entry(&rebuild::end, "end"),
    mapping::entry(&rebuild::id, "id", mapping::entry::invalid_on_zero),
    mapping::entry(&rebuild::is_index, "is_index"), mapping::entry()};

// Operations.
static io::data* new_rebuild() {
  return (new rebuild);
}
io::event_info::event_operations const rebuild::operations = {&new_rebuild};
