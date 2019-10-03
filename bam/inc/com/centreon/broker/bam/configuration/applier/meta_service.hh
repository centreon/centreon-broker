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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_META_SERVICE_HH
#define CCB_BAM_CONFIGURATION_APPLIER_META_SERVICE_HH

#include <map>
#include <memory>
#include "com/centreon/broker/bam/configuration/meta_service.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/meta_service.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace neb {
class host;
class service;
}  // namespace neb

namespace bam {
class metric_book;

namespace configuration {
namespace applier {
/**
 *  @class meta_service meta_service.hh
 * "com/centreon/broker/bam/configuration/applier/meta_service.hh"
 *  @brief Apply meta-services.
 *
 *  Create, update and delete meta-services.
 */
class meta_service {
 public:
  meta_service();
  meta_service(meta_service const& other);
  ~meta_service();
  meta_service& operator=(meta_service const& other);
  void apply(configuration::state::meta_services const& my_meta,
             metric_book& book);
  std::shared_ptr<bam::meta_service> find_meta(uint32_t id);

 private:
  struct applied {
    configuration::meta_service cfg;
    std::shared_ptr<bam::meta_service> obj;
  };

  void _internal_copy(meta_service const& other);
  std::shared_ptr<neb::host> _meta_host(uint32_t host_id);
  std::shared_ptr<neb::service> _meta_service(uint32_t meta_id,
                                              uint32_t host_id,
                                              uint32_t service_id);
  void _modify_meta(bam::meta_service& obj,
                    metric_book& book,
                    configuration::meta_service const& old_cfg,
                    configuration::meta_service const& new_cfg);
  std::shared_ptr<bam::meta_service> _new_meta(
      configuration::meta_service const& cfg,
      metric_book& book);

  std::map<uint32_t, applied> _applied;
};
}  // namespace applier
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_APPLIER_META_SERVICE_HH
