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
#include "com/centreon/broker/neb/statistics/services.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
services::services() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
services::services(services const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
services::~services() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
services& services::operator=(services const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string services::run() {
  // Count services ok / warning / unknown / critical.
  unsigned int total[4] = { 0, 0, 0, 0 };
  for (service* s(service_list); s; s = s->next)
    ++total[s->current_state];

  unsigned int not_ok(
                 total[STATE_WARNING]
                 + total[STATE_CRITICAL]
                 + total[STATE_UNKNOWN]);

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total[STATE_OK] << " services on status OK and "
      << not_ok << " services on non-OK status"
    "|ok=" << total[STATE_OK]
      << " warning=" << total[STATE_WARNING]
      << " critical=" << total[STATE_CRITICAL]
      << " unknown=" << total[STATE_UNKNOWN] << "\n";
  return (oss.str());
}
