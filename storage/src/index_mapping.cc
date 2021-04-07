/*
** Copyright 2009-2020 Centreon
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

#include "com/centreon/broker/storage/index_mapping.hh"

#include <cmath>

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

index_mapping::index_mapping()
    : io::data(index_mapping::static_type()),
      index_id(0),
      host_id(0),
      service_id(0) {}

/**
 *  Constructor
 *
 * @param index_id
 * @param host_id
 * @param service_id
 */
index_mapping::index_mapping(uint64_t index_id,
                             uint32_t host_id,
                             uint32_t service_id)
    : io::data(index_mapping::static_type()),
      index_id{index_id},
      host_id{host_id},
      service_id{service_id} {}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const index_mapping::entries[] = {
    mapping::entry(&index_mapping::index_id,
                   "index_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&index_mapping::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&index_mapping::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_index_mapping() {
  return new index_mapping;
}
io::event_info::event_operations const index_mapping::operations = {
    &new_index_mapping};
