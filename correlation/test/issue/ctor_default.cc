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
 *  Check that issue is properly default-constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  correlation::issue i;

  // Check default construction.
  return ((i.ack_time != 0)
          || (i.end_time != 0)
          || (i.host_id != 0)
          || (i.service_id != 0)
          || (i.start_time != 0)
          || (i.type()
              != "com::centreon::broker::correlation::issue"));
}
