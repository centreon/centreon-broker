/*
** Copyright 2015 Centreon
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

#ifndef CCB_GRAPHITE_MACRO_CACHE_HH
#define CCB_GRAPHITE_MACRO_CACHE_HH

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "com/centreon/broker/io/factory.hh"
#include "com/centreon/broker/misc/pair.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"

CCB_BEGIN()

namespace graphite {
/**
 *  @class macro_cache macro_cache.hh
 * "com/centreon/broker/graphite/macro_cache.hh"
 *  @brief Data cache for graphite macro.
 */
class macro_cache {
 public:
  macro_cache(std::shared_ptr<persistent_cache> const& cache);
  ~macro_cache();

  void write(std::shared_ptr<io::data> const& data);

  storage::index_mapping const& get_index_mapping(uint64_t index_id) const;
  storage::metric_mapping const& get_metric_mapping(uint64_t metric_id) const;
  std::string const& get_host_name(uint64_t host_id) const;
  std::string const& get_service_description(uint64_t host_id,
                                             uint64_t service_id) const;
  std::string const& get_instance(uint64_t instance_id) const;

 private:
  macro_cache(macro_cache const& f);
  macro_cache& operator=(macro_cache const& f);

  void _process_instance(neb::instance const& in);
  void _process_host(neb::host const& h);
  void _process_service(neb::service const& s);
  void _process_index_mapping(storage::index_mapping const& im);
  void _process_metric_mapping(storage::metric_mapping const& mm);
  void _save_to_disk();

  std::shared_ptr<persistent_cache> _cache;
  std::unordered_map<uint64_t, neb::instance> _instances;
  std::unordered_map<uint64_t, neb::host> _hosts;
  std::unordered_map<std::pair<uint64_t, uint64_t>, neb::service> _services;
  std::unordered_map<uint64_t, storage::index_mapping> _index_mappings;
  std::unordered_map<uint64_t, storage::metric_mapping> _metric_mappings;
};
}  // namespace graphite

CCB_END()

#endif  // !CCB_GRAPHITE_MACRO_CACHE_HH
