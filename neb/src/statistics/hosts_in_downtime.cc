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

#include <sstream>
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/hosts_in_downtime.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts_in_downtime::hosts_in_downtime()
  : plugin("hosts_in_downtime") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts_in_downtime::hosts_in_downtime(hosts_in_downtime const& right)
 : plugin(right) {

}

/**
 *  Destructor.
 */
hosts_in_downtime::~hosts_in_downtime() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
hosts_in_downtime& hosts_in_downtime::operator=(hosts_in_downtime const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void hosts_in_downtime::run(
              std::string& output,
	      std::string& perfdata) {
  // Count hosts in downtime.
  unsigned int total(0);
  for (host* h(host_list); h; h = h->next)
    if (h->scheduled_downtime_depth)
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total << " hosts in downtime";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "hosts_in_downtime=" << total;
  perfdata = oss.str();

  return ;
}
