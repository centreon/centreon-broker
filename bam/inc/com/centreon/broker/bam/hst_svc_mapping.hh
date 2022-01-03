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

#ifndef CCB_BAM_HST_SVC_MAPPING_HH
#define CCB_BAM_HST_SVC_MAPPING_HH

#include "com/centreon/broker/misc/pair.hh"
#include <unordered_map>
#include <string>
#include <utility>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class hst_svc_mapping hst_svc_mapping.hh
 * "com/centreon/broker/bam/hst_svc_mapping.hh"
 *  @brief Link name to ID.
 *
 *  Allow to find an ID of a host or service by its name.
 */
class hst_svc_mapping {
  std::unordered_map<std::pair<std::string, std::string>, std::pair<uint32_t, uint32_t>>
      _mapping;

  std::unordered_map<std::pair<uint32_t, uint32_t>, bool> _activated_mapping;

 public:
  hst_svc_mapping() = default;
  ~hst_svc_mapping() noexcept = default;
  hst_svc_mapping(const hst_svc_mapping&) = delete;
  hst_svc_mapping& operator=(const hst_svc_mapping&) = delete;
  uint32_t get_host_id(std::string const& hst) const;
  std::pair<uint32_t, uint32_t> get_service_id(std::string const& hst,
                                               std::string const& svc) const;
  void set_host(std::string const& hst, uint32_t host_id);
  void set_service(std::string const& hst,
                   std::string const& svc,
                   uint32_t host_id,
                   uint32_t service_id,
                   bool activated);

  bool get_activated(uint32_t hst_id, uint32_t service_id) const;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_HST_SVC_MAPPING_HH
