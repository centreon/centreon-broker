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

#ifndef CCB_BAM_BA_STATUS_HH
#define CCB_BAM_BA_STATUS_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class ba_status ba_status.hh "com/centreon/broker/bam/ba_status.hh"
 *  @brief Update status of a BA.
 *
 *  Update the status of a BA, used to update the cfg_bam table.
 */
class ba_status : public io::data {
 public:
  ba_status();
  ba_status(ba_status const& other);
  ~ba_status();
  ba_status& operator=(ba_status const& other);
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bam, bam::de_ba_status>::value;
  }

  uint32_t ba_id;
  bool in_downtime;
  timestamp last_state_change;
  double level_acknowledgement;
  double level_downtime;
  double level_nominal;
  short state;
  bool state_changed;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(ba_status const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BA_STATUS_HH
