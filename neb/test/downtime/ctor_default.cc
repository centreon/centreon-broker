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

#include "com/centreon/broker/neb/downtime.hh"

using namespace com::centreon::broker;

/**
 *  Check downtime's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::downtime dwntm;

  // Check.
  return ((dwntm.actual_end_time != 0)
          || (dwntm.actual_start_time != 0)
          || (dwntm.author != "")
          || (dwntm.comment != "")
          || (dwntm.deletion_time != 0)
          || (dwntm.downtime_type != 0)
          || (dwntm.duration != 0)
          || (dwntm.end_time != 0)
          || (dwntm.entry_time != 0)
          || (dwntm.fixed != true)
          || (dwntm.host_id != 0)
          || (dwntm.instance_id != 0)
          || (dwntm.internal_id != 0)
          || (dwntm.service_id != 0)
          || (dwntm.start_time != 0)
          || (dwntm.triggered_by != 0)
          || (dwntm.was_cancelled != false)
          || (dwntm.was_started != false)
          || (dwntm.type()
              != "com::centreon::broker::neb::downtime"));
}
