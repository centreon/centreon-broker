/*
** Copyright 2009-2013 Merethis
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
#include "com/centreon/broker/neb/notification.hh"

using namespace com::centreon::broker;
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

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const notification::entries[] = {
  mapping::entry(
    &notification::contacts_notified,
    "contacts_notified",
    1),
  mapping::entry(
    &notification::end_time,
    "end_time",
    2),
  mapping::entry(
    &notification::escalated,
    "escalated",
    3),
  mapping::entry(
    &notification::host_id,
    "host_id",
    4,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &notification::notification_type,
    "type",
    5),
  mapping::entry(
    &notification::reason_type,
    "reason_type",
    6),
  mapping::entry(
    &notification::service_id,
    "service_id",
    7,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &notification::start_time,
    "start_time",
    8),
  mapping::entry(
    &notification::state,
    "state",
    9),
  mapping::entry(
    &notification::ack_author,
    "ack_author",
    10),
  mapping::entry(
    &notification::ack_data,
    "ack_data",
    11),
  mapping::entry(
    &notification::command_name,
    "command_name",
    12),
  mapping::entry(
    &notification::contact_name,
    "contact_name",
    13),
  mapping::entry(
    &notification::output,
    "output",
    14),
  mapping::entry(
    &notification::instance_id,
    "",
    15),
  mapping::entry()
};

// Operations.
static io::data* new_notification() {
  return (new notification);
}
io::event_info::event_operations const notification::operations = {
  &new_notification
};
