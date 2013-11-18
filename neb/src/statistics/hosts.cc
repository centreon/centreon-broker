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
#include "com/centreon/broker/neb/statistics/hosts.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
hosts::hosts() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
hosts::hosts(hosts const& right) {
  (void)right;
}

/**
 *  Destructor.
 */
hosts::~hosts() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
hosts& hosts::operator=(hosts const& right) {
  (void)right;
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @return Statistics output.
 */
std::string hosts::run() {
  // Count hosts up / down / unreachable.
  unsigned int total[3] = { 0, 0, 0 };
  for (host* h(host_list); h; h = h->next)
    ++total[h->current_state];

  unsigned int not_up(total[HOST_DOWN] + total[HOST_UNREACHABLE]);

  // Output.
  std::ostringstream oss;
  oss << "Engine " << instance_name.toStdString()
      << " has " << total[HOST_UP] << " hosts on status UP and "
      << not_up << " hosts on non-UP status"
    "|up=" << total[HOST_UP]
      << " down=" << total[HOST_DOWN]
      << " unreachable=" << total[HOST_UNREACHABLE] << "\n";
  return (oss.str());
}
