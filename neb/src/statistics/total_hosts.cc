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

#include "com/centreon/broker/neb/statistics/total_hosts.hh"
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
total_hosts::total_hosts() : plugin("total_hosts") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
total_hosts::total_hosts(total_hosts const& right) : plugin(right) {}

/**
 *  Destructor.
 */
total_hosts::~total_hosts() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
total_hosts& total_hosts::operator=(total_hosts const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void total_hosts::run(std::string& output, std::string& perfdata) {
  // Count hosts.
  uint32_t total(com::centreon::engine::host::hosts.size());

  // Output.
  std::ostringstream oss;
  oss << "Engine " << config::applier::state::instance().poller_name()
      << " has " << total << " hosts";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "total_hosts=" << total;
  perfdata = oss.str();
}
