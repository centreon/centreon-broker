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
rebuild::rebuild()
    : io::data(rebuild::static_type()), end(true), id(0), is_index(false) {}

/**
 *  Constructor.
 *
 *  @param[in] ending   false if rebuild is starting, true if it is ending.
 *  @param[in] id       Index or metric ID.
 *  @param[in] is_index true for an index ID, false for a metric ID.
 */
rebuild::rebuild(bool ending, uint32_t id, bool is_index)
    : io::data(rebuild::static_type()),
      end(ending),
      id(id),
      is_index(is_index) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
rebuild::rebuild(rebuild const& right) : io::data(right) {
  _internal_copy(right);
}

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
  return *this;
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
  return new rebuild;
}
io::event_info::event_operations const rebuild::operations = {&new_rebuild};
