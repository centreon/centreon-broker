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

#include "com/centreon/broker/correlation/host_state.hh"

using namespace com::centreon::broker;

/**
 *  Check that host_state can be checked for non-equality.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  correlation::host_state hs1;
  hs1.ack_time = 6762;
  hs1.current_state = 2;
  hs1.end_time = 7456987;
  hs1.host_id = 21;
  hs1.in_downtime = true;
  hs1.service_id = 38475;
  hs1.start_time = 123456789;

  // Second object.
  correlation::host_state hs2(hs1);

  // Reset first object.
  correlation::host_state hs3;
  hs3.ack_time = 834957;
  hs3.current_state = 1;
  hs3.end_time = 5478963;
  hs3.host_id = 983;
  hs3.in_downtime = false;
  hs3.service_id = 471;
  hs3.start_time = 456887;

  // Check.
  return ((hs1 != hs2)
          || !(hs1 != hs3)
          || !(hs2 != hs3)
          || (hs1 != hs1)
          || (hs2 != hs2)
          || (hs3 != hs3));
}
