/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_NEB_DOWNTIME_MAP_HH
#  define CCB_NEB_DOWNTIME_MAP_HH

#  include <unordered_map>
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/neb/node_id.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/misc/pair.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class downtime_map downtime_map.hh "com/centreon/broker/neb/downtime_map.hh"
   *  @brief Map of downtimes.
   */
  class          downtime_map {
  public:
                 downtime_map();
                 downtime_map(
                   downtime_map const& other);
    downtime_map&
                 operator=(downtime_map const& other);
    virtual      ~downtime_map();

    unsigned int get_new_downtime_id();
    std::list<downtime>
                 get_all_downtimes_of_node(node_id id) const;
    std::list<downtime>
                 get_all_recurring_downtimes_of_node(node_id id) const;
    void         delete_downtime(downtime const& dwn);
    void         add_downtime(downtime const& dwn);
    downtime*    get_downtime(unsigned int internal_id);
    bool         is_recurring(unsigned int internal_id) const;
    std::list<downtime>
                 get_all_recurring_downtimes() const;
    std::list<downtime>
                 get_all_downtimes() const;
    bool         spawned_downtime_exist(unsigned int parent_id) const;

  private:
    unsigned int _actual_downtime_id;
    std::unordered_map<unsigned int, downtime>
                 _downtimes;
    std::unordered_multimap<node_id, unsigned int>
                 _downtime_id_by_nodes;
    std::unordered_map<unsigned int, downtime>
                 _recurring_downtimes;
    std::unordered_multimap<node_id, unsigned int>
                 _recurring_downtime_id_by_nodes;

  };
}

CCB_END()

#endif // !CCB_NEB_DOWNTIME_MAP_HH
