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

#include "com/centreon/broker/storage/remove_graph.hh"
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
remove_graph::remove_graph() : id(0), is_index(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
remove_graph::remove_graph(remove_graph const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
remove_graph::~remove_graph() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
remove_graph& remove_graph::operator=(remove_graph const& right) {
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
unsigned int remove_graph::type() const {
  return (remove_graph::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int remove_graph::static_type() {
  return (io::events::data_type<io::events::storage,
                                storage::de_remove_graph>::value);
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
void remove_graph::_internal_copy(remove_graph const& right) {
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
mapping::entry const remove_graph::entries[] = {
    mapping::entry(&remove_graph::id, "id", mapping::entry::invalid_on_zero),
    mapping::entry(&remove_graph::is_index, "is_end"), mapping::entry()};

// Operations.
static io::data* new_remove_graph() {
  return (new remove_graph);
}
io::event_info::event_operations const remove_graph::operations = {
    &new_remove_graph};
