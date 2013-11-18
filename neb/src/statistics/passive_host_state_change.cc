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
#include "com/centreon/broker/neb/statistics/passive_host_state_change.hh"
#include "com/centreon/broker/neb/statistics/compute_value.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
passive_host_state_change::passive_host_state_change() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
passive_host_state_change::passive_host_state_change(passive_host_state_change const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
passive_host_state_change::~passive_host_state_change() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
passive_host_state_change& passive_host_state_change::operator=(passive_host_state_change const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string passive_host_state_change::run() {
  std::ostringstream oss;
  compute_value<double> cv;
  for (host* h(host_list); h; h = h->next)
    if (h->check_type == HOST_CHECK_PASSIVE)
      cv << h->percent_state_change;
  if (cv.size()) {
    oss << "Engine " << instance_name.toStdString()
        << " has an average passive host state change of "
        << std::fixed << std::setprecision(2) << cv.avg()
        << "%|avg=" << cv.avg() << "% min=" << cv.min()
        << "% max=" << cv.max() << "%\n";
  }
  else {
    oss << "No passive host to compute passive host "
        << "state change on " << instance_name.toStdString() << "\n";
  }
  return (oss.str());
}
