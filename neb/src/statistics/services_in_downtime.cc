/*
** Copyright 2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <sstream>
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/services_in_downtime.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
services_in_downtime::services_in_downtime()
  : plugin("services_in_downtime") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
services_in_downtime::services_in_downtime(services_in_downtime const& right)
 : plugin(right) {

}

/**
 *  Destructor.
 */
services_in_downtime::~services_in_downtime() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
services_in_downtime& services_in_downtime::operator=(services_in_downtime const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void services_in_downtime::run(
              std::string& output,
	      std::string& perfdata) {
  // Count services in downtime.
  unsigned int total(0);
  for (service* s(service_list); s; s = s->next)
    if (s->scheduled_downtime_depth)
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total << " services in downtime";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "services_in_downtime=" << total;
  perfdata = oss.str();

  return ;
}
