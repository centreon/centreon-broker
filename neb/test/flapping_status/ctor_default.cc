/*
** Copyright 2012-2013 Merethis
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
#include "com/centreon/broker/neb/flapping_status.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check flapping_status's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::flapping_status flappy;

  // Check.
  return ((flappy.comment_time != 0)
          || (flappy.event_time != 0)
          || (flappy.event_type != 0)
          || (flappy.flapping_type != 0)
          || (fabs(flappy.high_threshold) > 0.000001)
          || (flappy.host_id != 0)
          || (flappy.internal_comment_id != 0)
          || (fabs(flappy.low_threshold) > 0.000001)
          || (fabs(flappy.percent_state_change) > 0.000001)
          || (flappy.reason_type != 0)
          || (flappy.service_id != 0)
          || (flappy.type()
              != io::data::data_type(io::data::neb, neb::de_flapping_status)));
}
