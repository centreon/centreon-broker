/*
** Copyright 2013 Centreon
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

#ifndef CCB_NEB_STATISTICS_ACTIVE_HOST_LATENCY_HH
#define CCB_NEB_STATISTICS_ACTIVE_HOST_LATENCY_HH

#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/statistics/plugin.hh"

CCB_BEGIN()

namespace neb {
namespace statistics {
/**
 *  @class active_host_latency active_host_latency.hh
 * "com/centreon/broker/neb/statistics/active_host_latency.hh"
 *  @brief active_host_latency statistics plugin.
 */
class active_host_latency : public plugin {
 public:
  active_host_latency();
  active_host_latency(active_host_latency const& right);
  ~active_host_latency();
  active_host_latency& operator=(active_host_latency const& right);
  void run(std::string& output, std::string& perfdata);
};
}  // namespace statistics
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_STATISTICS_ACTIVE_HOST_LATENCY_HH
