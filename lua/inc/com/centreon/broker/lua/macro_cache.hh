/*
** Copyright 2018 Centreon
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

#ifndef CCB_LUA_MACRO_CACHE_HH
#define CCB_LUA_MACRO_CACHE_HH

#include <map>
#include <memory>
#include <unordered_map>
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/misc/pair.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/host_group.hh"
#include "com/centreon/broker/neb/host_group_member.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_group.hh"
#include "com/centreon/broker/neb/service_group_member.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"

CCB_BEGIN()

namespace lua {
/**
 *  @class macro_cache macro_cache.hh "com/centreon/broker/lua/macro_cache.hh"
 *  @brief Data cache for Lua macro.
 */
class macro_cache {
 public:
  macro_cache(std::shared_ptr<persistent_cache> const& cache);
  ~macro_cache();

  void write(std::shared_ptr<io::data> const& data);

  storage::index_mapping const& get_index_mapping(uint32_t index_id) const;
  storage::metric_mapping const& get_metric_mapping(uint32_t metric_id)
      const;
  std::string const& get_host_name(uint64_t host_id) const;
  std::string const& get_notes_url(uint64_t host_id, uint64_t service_id) const;
  std::string const& get_notes(uint64_t host_id, uint64_t service_id) const;
  std::string const& get_action_url(uint64_t host_id,
                                    uint64_t service_id) const;
  std::string const& get_host_group_name(uint64_t id) const;
  std::map<std::pair<uint64_t, uint64_t>,
           std::shared_ptr<neb::host_group_member> > const&
      get_host_group_members() const;
  std::string const& get_service_description(uint64_t host_id,
                                             uint64_t service_id) const;
  std::string const& get_service_group_name(uint64_t id) const;
  std::map<std::tuple<uint64_t, uint64_t, uint64_t>,
           std::shared_ptr<neb::service_group_member> > const&
      get_service_group_members() const;
  std::string const& get_instance(uint64_t instance_id) const;

  std::unordered_multimap<
      uint64_t,
      std::shared_ptr<bam::dimension_ba_bv_relation_event> > const&
      get_dimension_ba_bv_relation_events() const;
  bam::dimension_ba_event const& get_dimension_ba_event(uint64_t id) const;
  bam::dimension_bv_event const& get_dimension_bv_event(uint64_t id) const;

 private:
  macro_cache(macro_cache const& f);
  macro_cache& operator=(macro_cache const& f);

  void _process_instance(std::shared_ptr<io::data> const& data);
  void _process_host(std::shared_ptr<io::data> const& data);
  void _process_host_group(std::shared_ptr<io::data> const& data);
  void _process_host_group_member(std::shared_ptr<io::data> const& data);
  void _process_service(std::shared_ptr<io::data> const& data);
  void _process_service_group(std::shared_ptr<io::data> const& data);
  void _process_service_group_member(std::shared_ptr<io::data> const& data);
  void _process_index_mapping(std::shared_ptr<io::data> const& data);
  void _process_metric_mapping(std::shared_ptr<io::data> const& data);
  void _process_dimension_ba_event(std::shared_ptr<io::data> const& data);
  void _process_dimension_ba_bv_relation_event(
      std::shared_ptr<io::data> const& data);
  void _process_dimension_bv_event(std::shared_ptr<io::data> const& data);
  void _process_dimension_truncate_table_signal(
      std::shared_ptr<io::data> const& data);

  void _save_to_disk();

  std::shared_ptr<persistent_cache> _cache;
  std::unordered_map<uint64_t, std::shared_ptr<neb::instance> > _instances;
  std::unordered_map<uint64_t, std::shared_ptr<neb::host> > _hosts;
  std::unordered_map<uint64_t, std::shared_ptr<neb::host_group> > _host_groups;
  std::map<std::pair<uint64_t, uint64_t>,
           std::shared_ptr<neb::host_group_member> > _host_group_members;
  std::unordered_map<std::pair<uint64_t, uint64_t>,
                     std::shared_ptr<neb::service> > _services;
  std::unordered_map<uint64_t, std::shared_ptr<neb::service_group> >
      _service_groups;
  std::map<std::tuple<uint64_t, uint64_t, uint64_t>,
           std::shared_ptr<neb::service_group_member> > _service_group_members;
  std::unordered_map<uint64_t, std::shared_ptr<storage::index_mapping> >
      _index_mappings;
  std::unordered_map<uint64_t, std::shared_ptr<storage::metric_mapping> >
      _metric_mappings;
  std::unordered_map<uint64_t, std::shared_ptr<bam::dimension_ba_event> >
      _dimension_ba_events;
  std::unordered_multimap<uint64_t,
                          std::shared_ptr<bam::dimension_ba_bv_relation_event> >
      _dimension_ba_bv_relation_events;
  std::unordered_map<uint64_t, std::shared_ptr<bam::dimension_bv_event> >
      _dimension_bv_events;
};
}  // namespace lua

CCB_END()

#endif  // !CCB_LUA_MACRO_CACHE_HH
