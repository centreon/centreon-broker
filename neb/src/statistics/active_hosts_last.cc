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

#include "com/centreon/broker/neb/statistics/active_hosts_last.hh"
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;
using namespace com::centreon::engine;

/**
 *  Default constructor.
 */
active_hosts_last::active_hosts_last() : plugin("active_hosts_last") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
active_hosts_last::active_hosts_last(active_hosts_last const& right)
    : plugin(right) {}

/**
 *  Destructor.
 */
active_hosts_last::~active_hosts_last() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
active_hosts_last& active_hosts_last::operator=(
    active_hosts_last const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void active_hosts_last::run(std::string& output, std::string& perfdata) {
  uint32_t last_checked_1(0);
  uint32_t last_checked_5(0);
  uint32_t last_checked_15(0);
  uint32_t last_checked_60(0);
  time_t now(time(nullptr));
  for (host_map::const_iterator it{com::centreon::engine::host::hosts.begin()},
       end{com::centreon::engine::host::hosts.end()};
       it != end; ++it) {
    if (it->second->get_check_type() == checkable::check_active) {
      int diff(now - it->second->get_last_check());
      if (diff <= 60 * 60) {
        ++last_checked_60;
        if (diff <= 15 * 60) {
          ++last_checked_15;
          if (diff <= 5 * 60) {
            ++last_checked_5;
            if (diff <= 1 * 60)
              ++last_checked_1;
          }
        }
      }
    }
  }

  // Output.
  std::ostringstream oss;
  oss << "Engine " << config::applier::state::instance().poller_name()
      << " ran " << last_checked_5
      << " active checks during the last 5 minutes";
  output = oss.str();

  // Perdata.
  oss.str("");
  oss << "active_hosts_last_1=" << last_checked_1
      << " active_hosts_last_5=" << last_checked_5
      << " active_hosts_last_15=" << last_checked_15
      << " active_hosts_last_60=" << last_checked_60;
  perfdata = oss.str();

  return;
}
