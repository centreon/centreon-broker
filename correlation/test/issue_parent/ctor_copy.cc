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

#include "com/centreon/broker/correlation/issue_parent.hh"

using namespace com::centreon::broker;

/**
 *  Check that issue_parent can be properly copy-constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  correlation::issue_parent ip1;
  ip1.child_host_id = 48745748;
  ip1.child_service_id = 1;
  ip1.child_start_time = 3148514254u;
  ip1.end_time = 7456987;
  ip1.parent_host_id = 4578;
  ip1.parent_service_id = 42;
  ip1.parent_start_time = 72115;
  ip1.start_time = 123456789;

  // Second object.
  correlation::issue_parent ip2(ip1);

  // Reset first object.
  ip1.child_host_id = 3;
  ip1.child_service_id = 548784;
  ip1.child_start_time = 215;
  ip1.end_time = 5478963;
  ip1.parent_host_id = 100;
  ip1.parent_service_id = 787844;
  ip1.parent_start_time = 666;
  ip1.start_time = 456887;

  // Check.
  return ((ip1.child_host_id != 3)
          || (ip1.child_service_id != 548784)
          || (ip1.child_start_time != 215)
          || (ip1.end_time != 5478963)
          || (ip1.parent_host_id != 100)
          || (ip1.parent_service_id != 787844)
          || (ip1.parent_start_time != 666)
          || (ip1.start_time != 456887)
          || (ip2.child_host_id != 48745748)
          || (ip2.child_service_id != 1)
          || (ip2.child_start_time != static_cast<time_t>(3148514254u))
          || (ip2.end_time != 7456987)
          || (ip2.parent_host_id != 4578)
          || (ip2.parent_service_id != 42)
          || (ip2.parent_start_time != 72115)
          || (ip2.start_time != 123456789));
}
