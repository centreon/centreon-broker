/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_DIMENSION_BV_EVENT_HH
#define CCB_BAM_DIMENSION_BV_EVENT_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class dimension_bv_event dimension_bv_event.hh
 * "com/centreon/broker/bam/dimension_bv_event.hh"
 *  @brief Dimension Bv event
 *
 */
class dimension_bv_event : public io::data {
 public:
  dimension_bv_event();
  dimension_bv_event(dimension_bv_event const& other);
  ~dimension_bv_event();
  dimension_bv_event& operator=(dimension_bv_event const& other);
  bool operator==(dimension_bv_event const& other) const;
  uint32_t type() const;
  static uint32_t static_type();

  uint32_t bv_id;
  std::string bv_name;
  std::string bv_description;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(dimension_bv_event const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_DIMENSION_BV_EVENT_HH
