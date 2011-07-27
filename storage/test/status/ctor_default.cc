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

#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;

/**
 *  Check that the status object properly default constructs.
 *
 *  @return 0 on success.
 */
int main() {
  // Build object.
  storage::status s;

  // Check properties values.
  return ((s.ctime != 0)
          || (s.index_id != 0)
          || (s.interval != 0)
          || (s.rrd_len != 0)
          || (s.state != 0)
          || (s.type() != "com::centreon::broker::storage::status"));
}
