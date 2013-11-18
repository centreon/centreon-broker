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
#include "com/centreon/broker/neb/statistics/active_hosts_last.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
active_hosts_last::active_hosts_last()
  : plugin("active_hosts_last") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
active_hosts_last::active_hosts_last(active_hosts_last const& right)
 : plugin(right) {

}

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
active_hosts_last& active_hosts_last::operator=(active_hosts_last const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void active_hosts_last::run(
              std::string& output,
	      std::string& perfdata) {
  unsigned int last_checked_1(0);
  unsigned int last_checked_5(0);
  unsigned int last_checked_15(0);
  unsigned int last_checked_60(0);
  time_t now(time(NULL));
  for (host* h(host_list); h; h = h->next) {
    if (h->check_type == HOST_CHECK_ACTIVE) {
      int diff(now - h->last_check);
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
  oss << "Engine " << instance_name.toStdString() << " ran "
      << last_checked_5 << " active checks during the last 5 minutes";
  output = oss.str();

  // Perdata.
  oss.str("");
  oss << "active_hosts_last_1=" << last_checked_1
      << " active_hosts_last_5=" << last_checked_5
      << " active_hosts_last_15=" << last_checked_15
      << " active_hosts_last_60=" << last_checked_60;
  perfdata = oss.str();

  return ;
}
