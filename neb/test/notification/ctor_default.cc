/*
** Copyright 2012-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/io/events.hh"
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
          || (notif.instance_id != 0)
          || (notif.notification_type != 0)
          || (notif.output != "")
          || (notif.reason_type != 0)
          || (notif.service_id != 0)
          || (notif.start_time != 0)
          || (notif.state != 0)
          || (notif.type()
              != io::events::data_type<io::events::neb, neb::de_notification>::value));
}
