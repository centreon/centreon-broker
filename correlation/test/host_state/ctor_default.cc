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
 *  Check that host_state is properly default-constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  correlation::host_state hs;

  // Check default construction.
  return ((hs.ack_time != 0)
          || (hs.current_state != 3)
          || (hs.end_time != 0)
          || (hs.host_id != 0)
          || (hs.in_downtime != false)
          || (hs.service_id != 0)
          || (hs.start_time != 0)
          || (hs.type()
              != "com::centreon::broker::correlation::host_state"));
}
