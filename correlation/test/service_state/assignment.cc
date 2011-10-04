/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/correlation/service_state.hh"

using namespace com::centreon::broker;

/**
 *  Check that service_state can be properly assigned.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  correlation::service_state ss1;
  ss1.current_state = 2;
  ss1.end_time = 7456987;
  ss1.host_id = 21;
  ss1.in_downtime = true;
  ss1.service_id = 42;
  ss1.start_time = 123456789;

  // Second object.
  correlation::service_state ss2;
  ss2 = ss1;

  // Reset first object.
  ss1.current_state = 1;
  ss1.end_time = 5478963;
  ss1.host_id = 983;
  ss1.in_downtime = false;
  ss1.service_id = 211;
  ss1.start_time = 456887;

  // Check.
  return ((ss1.current_state != 1)
          || (ss1.end_time != 5478963)
          || (ss1.host_id != 983)
          || (ss1.in_downtime != false)
          || (ss1.service_id != 211)
          || (ss1.start_time != 456887)
          || (ss2.current_state != 2)
          || (ss2.end_time != 7456987)
          || (ss2.host_id != 21)
          || (ss2.in_downtime != true)
          || (ss2.service_id != 42)
          || (ss2.start_time != 123456789));
}
