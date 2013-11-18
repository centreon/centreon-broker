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
#include "com/centreon/broker/neb/statistics/services_passively_checked.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
services_passively_checked::services_passively_checked() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
services_passively_checked::services_passively_checked(services_passively_checked const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
services_passively_checked::~services_passively_checked() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
services_passively_checked& services_passively_checked::operator=(services_passively_checked const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string services_passively_checked::run() {
  // Count services passively checked.
  unsigned int total(0);
  for (service* s(service_list); s; s = s->next)
    if (s->accept_passive_service_checks)
      ++total;

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total << " services passively checked"
    "|services_passively_checked=" << total << "\n";
  return (oss.str());
}
