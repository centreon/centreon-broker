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
 *  Check that an initial service state log is properly parsed.
 *
 *  @return 0 on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse an initial service state line.
  neb::set_log_data(le,
    "INITIAL SERVICE STATE: myserver;myservice;UNKNOWN;SOFT;1;ERROR when getting SNMP version");

  // Check that it was properly parsed.
  return ((le.host_name != "myserver")
          || (le.log_type != 0) // SOFT
          || (le.msg_type != 8) // INITIAL SERVICE STATE
          || (le.output != "ERROR when getting SNMP version")
          || (le.retry != 1)
          || (le.service_description != "myservice")
          || (le.status != 3)); // UNKNOWN
}
