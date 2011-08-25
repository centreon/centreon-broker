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
 *  Check that a service alert log is properly parsed.
 *
 *  @return 0 on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse a service alert line.
  neb::set_log_data(le,
    "SERVICE ALERT: myserver;myservice;WARNING;SOFT;3;CPU 84%");

  // Check that it was properly parsed.
  return ((le.host_name != "myserver")
          || (le.log_type != 0) // SOFT
          || (le.msg_type != 0) // SERVICE ALERT
          || (le.output != "CPU 84%")
          || (le.retry != 3)
          || (le.service_description != "myservice")
          || (le.status != 1)); // WARNING
}
