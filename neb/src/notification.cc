/*
** Copyright 2009-2013 Centreon
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

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
notification::notification()
  : contacts_notified(false),
    end_time(0),
    escalated(false),
    host_id(0),
        notification_type(0),
    reason_type(0),
    service_id(0),
    start_time(0),
    state(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] n Object to copy.
 */
notification::notification(notification const& n) : io::data(n) {
  _internal_copy(n);
}

/**
 *  Destructor.
 */
notification::~notification() {}

/**
 *  Assignment operator.
 *
 *  @param[in] n Object to copy.
 *
 *  @return This object.
 */
notification& notification::operator=(notification const& n) {
  io::data::operator=(n);
  _internal_copy(n);
  return (*this);
}

/**
 *  Get the type of this object.
 *
 *  @return The event_type.
 */
unsigned int notification::type() const {
  return (io::events::data_type<io::events::neb, neb::de_notification>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] n Object to copy.
 */
void notification::_internal_copy(notification const& n) {
  ack_author = n.ack_author;
  ack_data = n.ack_data;
  command_name = n.command_name;
  contact_name = n.contact_name;
  contacts_notified = n.contacts_notified;
  end_time = n.end_time;
  escalated = n.escalated;
  host_id = n.host_id;
    notification_type = n.notification_type;
  output = n.output;
  reason_type = n.reason_type;
  service_id = n.service_id;
  start_time = n.start_time;
  state = n.state;
  return ;
}
