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

#include "com/centreon/broker/neb/statistics/hosts.hh"
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts::hosts() : plugin("hosts") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts::hosts(hosts const& right) : plugin(right) {}

/**
 *  Destructor.
 */
hosts::~hosts() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
hosts& hosts::operator=(hosts const& right) {
  plugin::operator=(right);
  return *this;
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void hosts::run(std::string& output, std::string& perfdata) {
  // Count hosts up / down / unreachable.
  uint32_t total[3] = {0, 0, 0};
  for (host_map::const_iterator it{com::centreon::engine::host::hosts.begin()},
       end{com::centreon::engine::host::hosts.end()};
       it != end; ++it)
    ++total[it->second->get_current_state()];

  uint32_t not_up{total[com::centreon::engine::host::state_down] +
                      total[com::centreon::engine::host::state_unreachable]};

  // Output.
  std::ostringstream oss;
  oss << "Engine " << config::applier::state::instance().poller_name()
      << " has " << total[com::centreon::engine::host::state_up]
      << " hosts on status UP and " << not_up << " hosts on non-UP status";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "up=" << total[com::centreon::engine::host::state_up]
      << " down=" << total[com::centreon::engine::host::state_down]
      << " unreachable="
      << total[com::centreon::engine::host::state_unreachable];
  perfdata = oss.str();
}
