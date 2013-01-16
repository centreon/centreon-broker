/*
** Copyright 2011-2012 Merethis
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

#include <cmath>
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the metric object properly default constructs.
 *
 *  @return 0 on success.
 */
int main() {
  // Build object.
  storage::metric m;

  // Check properties values.
  return ((m.ctime != 0)
          || (m.interval != 0)
          || (m.is_for_rebuild != false)
          || (m.metric_id != 0)
          || !m.name.isEmpty()
          || (m.rrd_len != 0)
          || !isnan(m.value)
          || (m.value_type != storage::perfdata::gauge)
          || (m.type() != "com::centreon::broker::storage::metric"));
}
