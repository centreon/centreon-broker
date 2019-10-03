/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_BA_HH
#define CCB_BAM_CONFIGURATION_APPLIER_BA_HH

#include <map>
#include <memory>
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/configuration/ba.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

// Forward declarations.
namespace neb {
class host;
class service;
}  // namespace neb

namespace bam {
namespace configuration {
namespace applier {
/**
 *  @class ba ba.hh "com/centreon/broker/bam/configuration/applier/ba.hh"
 *  @brief Apply BA configuration.
 *
 *  Take the configuration of BAs and apply it.
 */
class ba {
 public:
  ba();
  ba(ba const& other);
  ~ba();
  ba& operator=(ba const& other);
  void apply(configuration::state::bas const& my_bas, service_book& book);
  std::shared_ptr<bam::ba> find_ba(uint32_t id);
  void visit(io::stream* visitor);
  void save_to_cache(persistent_cache& cache);
  void load_from_cache(persistent_cache& cache);

 private:
  struct applied {
    configuration::ba cfg;
    std::shared_ptr<bam::ba> obj;
  };

  std::shared_ptr<neb::host> _ba_host(uint32_t host_id);
  std::shared_ptr<neb::service> _ba_service(uint32_t ba_id,
                                            uint32_t host_id,
                                            uint32_t service_id);
  void _internal_copy(ba const& other);
  std::shared_ptr<bam::ba> _new_ba(configuration::ba const& cfg,
                                   service_book& book);

  std::map<uint32_t, applied> _applied;
};
}  // namespace applier
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_APPLIER_BA_HH
