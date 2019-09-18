/*
** Copyright 2015 - 2019 Centreon
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

#ifndef CCB_STORAGE_INDEX_MAPPING_HH
#define CCB_STORAGE_INDEX_MAPPING_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace storage {
/**
 *  @class index_mapping index_mapping.hh
 * "com/centreon/broker/storage/index_mapping.hh"
 *  @brief Information about an index stored in the database.
 *
 *  Used to provide more informations about the mapping of the index
 *  to its service/host.
 */
class index_mapping : public io::data {
 public:
  index_mapping();
  index_mapping(uint32_t index_id, uint32_t host_id, uint32_t service_id);
  index_mapping(index_mapping const& other);
  ~index_mapping();
  index_mapping& operator=(index_mapping const& other);
  unsigned int type() const;
  static unsigned int static_type();

  uint32_t index_id;
  uint32_t host_id;
  uint32_t service_id;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(index_mapping const& other);
};
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_INDEX_MAPPING_HH
