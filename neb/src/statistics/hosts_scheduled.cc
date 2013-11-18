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
#include "com/centreon/broker/neb/statistics/hosts_scheduled.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts_scheduled::hosts_scheduled() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts_scheduled::hosts_scheduled(hosts_scheduled const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
hosts_scheduled::~hosts_scheduled() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
hosts_scheduled& hosts_scheduled::operator=(hosts_scheduled const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string hosts_scheduled::run() {
  // Count hosts scheduled.
  unsigned int total(0);
  for (host* h(host_list); h; h = h->next)
    if (h->should_be_scheduled)
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total << " scheduled hosts"
    "|hosts_scheduled=" << total << "\n";
  return (oss.str());
}
