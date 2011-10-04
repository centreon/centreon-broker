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

#include "com/centreon/broker/correlation/issue.hh"

using namespace com::centreon::broker;

/**
 *  Check that issue can be properly copy-constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  correlation::issue i1;
  i1.ack_time = 248578;
  i1.end_time = 7456987;
  i1.host_id = 21;
  i1.service_id = 42;
  i1.start_time = 123456789;

  // Second object.
  correlation::issue i2(i1);

  // Reset first object.
  i1.ack_time = 1684474;
  i1.end_time = 5478963;
  i1.host_id = 983;
  i1.service_id = 211;
  i1.start_time = 456887;

  // Check.
  return ((i1.ack_time != 1684474)
          || (i1.end_time != 5478963)
          || (i1.host_id != 983)
          || (i1.service_id != 211)
          || (i1.start_time != 456887)
          || (i2.ack_time != 248578)
          || (i2.end_time != 7456987)
          || (i2.host_id != 21)
          || (i2.service_id != 42)
          || (i2.start_time != 123456789));
}
