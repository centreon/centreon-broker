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

#include <iomanip>
#include <sstream>
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/active_host_state_change.hh"
#include "com/centreon/broker/neb/statistics/compute_value.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
active_host_state_change::active_host_state_change()
  : plugin("active_host_state_change") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
active_host_state_change::active_host_state_change(active_host_state_change const& right)
 : plugin(right) {

}

/**
 *  Destructor.
 */
active_host_state_change::~active_host_state_change() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
active_host_state_change& active_host_state_change::operator=(active_host_state_change const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void active_host_state_change::run(
              std::string& output,
	      std::string& perfdata) {
  compute_value<double> cv;
  for (host* h(host_list); h; h = h->next)
    if (h->check_type == HOST_CHECK_ACTIVE)
      cv << h->percent_state_change;

  if (cv.size()) {
    // Output.
    std::ostringstream oss;
    oss << "Engine " << instance_name.toStdString()
        << " has an average active host state change of "
        << std::fixed << std::setprecision(2) << cv.avg() << "%";
    output = oss.str();

    // Perfdata.
    oss.str("");
    oss << "avg=" << cv.avg() << "% min=" << cv.min()
        << "% max=" << cv.max() << "%";
    perfdata = oss.str();
  }
  else {
    // Output.
    output = "No active host to compute active host state "
      "change on " + instance_name.toStdString();
  }
  return ;
}
