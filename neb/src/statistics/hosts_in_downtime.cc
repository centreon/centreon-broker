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
#include "com/centreon/broker/neb/statistics/hosts_in_downtime.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts_in_downtime::hosts_in_downtime() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts_in_downtime::hosts_in_downtime(hosts_in_downtime const& right) {
  (void)right;
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
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string hosts_in_downtime::run() {
  // Count hosts in downtime.
  unsigned int total(0);
  for (host* h(host_list); h; h = h->next)
    if (h->scheduled_downtime_depth)
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total << " hosts in downtime"
    "|hosts_in_downtime=" << total << "\n";
  return (oss.str());
}
