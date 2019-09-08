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

#ifndef CCB_BAM_BA_SVC_MAPPING_HH
#define CCB_BAM_BA_SVC_MAPPING_HH

#include <map>
#include <string>
#include <utility>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class ba_svc_mapping ba_svc_mapping.hh
 * "com/centreon/broker/bam/ba_svc_mapping.hh"
 *  @brief Link BA ID to host name and service description.
 *
 *  Allow users to get a virtual BA host name and service description
 *  by a BA ID.
 */
class ba_svc_mapping {
 public:
  ba_svc_mapping();
  ba_svc_mapping(ba_svc_mapping const& other);
  ~ba_svc_mapping();
  ba_svc_mapping& operator=(ba_svc_mapping const& other);
  std::pair<std::string, std::string> get_service(unsigned int ba_id);
  void set(unsigned int ba_id, std::string const& hst, std::string const& svc);

 private:
  void _internal_copy(ba_svc_mapping const& other);

  std::map<unsigned int, std::pair<std::string, std::string> > _mapping;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BA_SVC_MAPPING_HH
