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
#include "com/centreon/broker/neb/statistics/passive_service_state_change.hh"
#include "com/centreon/broker/neb/statistics/compute_value.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
passive_service_state_change::passive_service_state_change() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
passive_service_state_change::passive_service_state_change(passive_service_state_change const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
passive_service_state_change::~passive_service_state_change() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
passive_service_state_change& passive_service_state_change::operator=(passive_service_state_change const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string passive_service_state_change::run() {
  std::ostringstream oss;
  compute_value<double> cv;
  for (service* s(service_list); s; s = s->next)
    if (s->check_type == SERVICE_CHECK_PASSIVE)
      cv << s->percent_state_change;
  if (cv.size()) {
    oss << "Engine " << instance_name.toStdString()
        << " has an average passive service state change of "
        << std::fixed << std::setprecision(2) << cv.avg()
        << "%|avg=" << cv.avg() << "% min=" << cv.min()
        << "% max=" << cv.max() << "%\n";
  }
  else {
    oss << "No passive service to compute passive service "
        << "state change on " << instance_name.toStdString() << "\n";
  }
  return (oss.str());
}
