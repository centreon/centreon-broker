/*
** Copyright 2013,2015 Merethis
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
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/hosts_scheduled.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts_scheduled::hosts_scheduled()
  : plugin("hosts_scheduled") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts_scheduled::hosts_scheduled(hosts_scheduled const& right)
 : plugin(right) {

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
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void hosts_scheduled::run(
              std::string& output,
	      std::string& perfdata) {
  // Count hosts scheduled.
  unsigned int total(0);
  for (host* h(host_list); h; h = h->next)
    if (h->should_be_scheduled)
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << config::applier::state::instance().poller_name()
      << " has " << total << " scheduled hosts";
  output = oss.str();

  // Perfdata.
  oss.str("");
  oss << "hosts_scheduled=" << total;
  perfdata = oss.str();

  return ;
}
