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

#ifndef CCB_NEB_STATISTICS_PASSIVE_HOST_STATE_CHANGE_HH
#define CCB_NEB_STATISTICS_PASSIVE_HOST_STATE_CHANGE_HH

#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/statistics/plugin.hh"

CCB_BEGIN()

namespace neb {
namespace statistics {
/**
 *  @class passive_host_state_change passive_host_state_change.hh
 * "com/centreon/broker/neb/statistics/passive_host_state_change.hh"
 *  @brief passive_host_state_change statistics plugin.
 */
class passive_host_state_change : public plugin {
 public:
  passive_host_state_change();
  passive_host_state_change(passive_host_state_change const& right);
  ~passive_host_state_change();
  passive_host_state_change& operator=(passive_host_state_change const& right);
  void run(std::string& output, std::string& perfdata);
};
}  // namespace statistics
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_STATISTICS_PASSIVE_HOST_STATE_CHANGE_HH
