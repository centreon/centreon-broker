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

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Defaut constructor
 */
remove_graph::remove_graph()
    : io::data(remove_graph::static_type()), id{0}, is_index{0} {}

/**
 *  Constructor
 *
 * @param index_id an index id
 * @param is_index a boolean telling if it is an index
 */
remove_graph::remove_graph(uint32_t index_id, bool is_index)
    : io::data(remove_graph::static_type()), id{index_id}, is_index{is_index} {}

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
  return new remove_graph;
}
io::event_info::event_operations const remove_graph::operations = {
    &new_remove_graph};
