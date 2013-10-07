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

#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/notification.hh"

using namespace com::centreon::broker;

/**
 *  Check notification's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::notification notif;

  // Check.
  return ((notif.ack_author != "")
          || (notif.ack_data != "")
          || (notif.command_name != "")
          || (notif.contact_name != "")
          || (notif.contacts_notified != false)
          || (notif.end_time != 0)
          || (notif.escalated != false)
          || (notif.host_id != 0)
          || (notif.notification_type != 0)
          || (notif.output != "")
          || (notif.reason_type != 0)
          || (notif.service_id != 0)
          || (notif.start_time != 0)
          || (notif.state != 0)
          || (notif.type()
              != io::data::data_type(io::data::neb, neb::de_notification)));
}
