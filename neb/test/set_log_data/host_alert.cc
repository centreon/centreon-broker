/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that a host alert log is properly parsed.
 *
 *  @return 0 on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse a host alert line.
  neb::set_log_data(le,
    "HOST ALERT: myserver;UNREACHABLE;HARD;4;Time to live exceeded");

  // Check that it was properly parsed.
  return ((le.host_name != "myserver")
          || (le.log_type != 1) // HARD
          || (le.msg_type != 1) // HOST ALERT
          || (le.output != "Time to live exceeded")
          || (le.retry != 4)
          || (le.status != 2)); // UNREACHABLE
}
