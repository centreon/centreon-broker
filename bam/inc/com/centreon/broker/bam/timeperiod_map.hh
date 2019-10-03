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

#ifndef CCB_BAM_TIMEPERIOD_MAP_HH
#define CCB_BAM_TIMEPERIOD_MAP_HH

#include <map>
#include <memory>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class timeperiod_map timeperiod_map.hh
 * "com/centreon/broker/bam/timeperiod_map.hh"
 *  @brief Timeperiod map made to be shared between threads.
 */
class timeperiod_map {
 public:
  timeperiod_map();
  ~timeperiod_map();
  timeperiod_map(timeperiod_map const&);
  timeperiod_map& operator=(timeperiod_map const&);
  bool operator==(timeperiod_map const& other) const;

  time::timeperiod::ptr get_timeperiod(uint32_t id) const;
  void add_timeperiod(uint32_t id, time::timeperiod::ptr ptr);
  void clear();
  void add_relation(uint32_t ba_id,
                    uint32_t timeperiod_id,
                    bool is_default);
  std::vector<std::pair<time::timeperiod::ptr, bool> > get_timeperiods_by_ba_id(
      uint32_t ba_id) const;

 private:
  std::map<uint32_t, time::timeperiod::ptr> _map;
  typedef std::multimap<uint32_t, std::pair<uint32_t, bool> >
      timeperiod_relation_map;
  timeperiod_relation_map _timeperiod_relations;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_TIMEPERIOD_MAP_HH
