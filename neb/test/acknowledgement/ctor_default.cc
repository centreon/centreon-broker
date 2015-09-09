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
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check acknowledgement's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::acknowledgement ack;

  // Check.
  return ((ack.acknowledgement_type != 0)
          || (ack.author != "")
          || (ack.comment != "")
          || (ack.deletion_time != 0)
          || (ack.entry_time != 0)
          || (ack.host_id != 0)
          || (ack.instance_id != 0)
          || (ack.is_sticky != false)
          || (ack.notify_contacts != false)
          || (ack.persistent_comment != false)
          || (ack.service_id != 0)
          || (ack.state != 0)
          || (ack.type()
              != io::events::data_type<io::events::neb, neb::de_acknowledgement>::value));
}
