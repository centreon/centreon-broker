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

#include "com/centreon/broker/neb/statistics/hosts_checked.hh"
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts_checked::hosts_checked() : plugin("hosts_checked") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts_checked::hosts_checked(hosts_checked const& right) : plugin(right) {}

/**
 *  Destructor.
 */
hosts_checked::~hosts_checked() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
hosts_checked& hosts_checked::operator=(hosts_checked const& right) {
  plugin::operator=(right);
  return *this;
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void hosts_checked::run(std::string& output, std::string& perfdata) {
  // Count hosts checked.
  uint32_t total{0};
  for (host_map::const_iterator it{com::centreon::engine::host::hosts.begin()},
       end{com::centreon::engine::host::hosts.end()};
       it != end; ++it)
    if (it->second->get_has_been_checked())
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << config::applier::state::instance().poller_name()
      << " has " << total << " checked hosts";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "hosts_checked=" << total;
  perfdata = oss.str();
}
