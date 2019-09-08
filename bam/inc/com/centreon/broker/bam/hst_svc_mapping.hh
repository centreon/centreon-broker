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

#include <map>
#include <set>
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
 public:
  hst_svc_mapping();
  hst_svc_mapping(hst_svc_mapping const& other);
  ~hst_svc_mapping();
  hst_svc_mapping& operator=(hst_svc_mapping const& other);
  unsigned int get_host_id(std::string const& hst) const;
  std::pair<unsigned int, unsigned int> get_service_id(
      std::string const& hst,
      std::string const& svc) const;
  void set_host(std::string const& hst, unsigned int host_id);
  void set_service(std::string const& hst,
                   std::string const& svc,
                   unsigned int host_id,
                   unsigned int service_id,
                   bool activated);
  void register_metric(unsigned int metric_id,
                       std::string const& metric_name,
                       unsigned int host_id,
                       unsigned int service_id);
  std::set<unsigned int> get_metric_ids(std::string const& metric_name,
                                        unsigned int host_id,
                                        unsigned int service_id) const;

  bool get_activated(unsigned int hst_id, unsigned int service_id) const;

 private:
  void _internal_copy(hst_svc_mapping const& other);

  std::map<std::pair<std::string, std::string>,
           std::pair<unsigned int, unsigned int> >
      _mapping;

  std::map<std::pair<unsigned int, unsigned int>, bool> _activated_mapping;

  std::map<std::pair<unsigned int, unsigned int>,
           std::map<std::string, unsigned int> >
      _metrics;
  std::multimap<std::string, unsigned int> _metric_by_name;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_HST_SVC_MAPPING_HH
