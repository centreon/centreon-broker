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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/log_entry.hh"

using namespace com::centreon::broker;

/**
 *  Check log_entry's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::log_entry le;

  // Check.
  return ((le.c_time != 0)
          || (le.host_id != 0)
          || (le.host_name != "")
          || (le.instance_id != 0)
          || (le.instance_name != "")
          || (le.issue_start_time != 0)
          || (le.log_type != 0)
          || (le.msg_type != 0)
          || (le.notification_cmd != "")
          || (le.notification_contact != "")
          || (le.output != "")
          || (le.retry != 0)
          || (le.service_description != "")
          || (le.service_id != 0)
          || (le.status != 5)
          || (le.type()
              != io::events::data_type<io::events::neb, neb::de_log_entry>::value));
}
