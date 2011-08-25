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
 *  Check that a current service state log is properly parsed.
 *
 *  @return 0 on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse a current service state line.
  neb::set_log_data(le,
    "CURRENT SERVICE STATE: myserver;myservice;UNKNOWN;HARD;2;Requested table is empty.");

  // Check that it was properly parsed.
  return ((le.host_name != "myserver")
          || (le.log_type != 1) // HARD
          || (le.msg_type != 6) // CURRENT_SERVICE_STATE
          || (le.output != "Requested table is empty.")
          || (le.retry != 2)
          || (le.status != 3)); // UNKNOWN
}
