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
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
log_entry::log_entry()
  : c_time(0),
    host_id(0),
    instance_id(0),
    issue_start_time(0),
    log_type(0),
    msg_type(0),
    retry(0),
    service_id(0),
    status(5) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] le Object to copy.
 */
log_entry::log_entry(log_entry const& le) : io::data(le) {
  _internal_copy(le);
}

/**
 *  Destructor.
 */
log_entry::~log_entry() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] le Object to copy.
 *
 *  @return This object.
 */
log_entry& log_entry::operator=(log_entry const& le) {
  io::data::operator=(le);
  _internal_copy(le);
  return (*this);
}

/**
 *  Returns the type of the event.
 *
 *  @return The event_type.
 */
unsigned int log_entry::type() const {
  return (io::events::data_type<io::events::neb, neb::de_log_entry>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy all internal data of the given object to the current
 *         instance.
 *
 *  Make a copy of all data defined within the log_entry class. This
 *  method is used by the copy constructor and the assignment operator.
 *
 *  @param[in] le Object to copy.
 */
void log_entry::_internal_copy(log_entry const& le) {
  c_time = le.c_time;
  host_id = le.host_id;
  host_name = le.host_name;
  instance_id = le.instance_id;
  instance_name = le.instance_name;
  issue_start_time = le.issue_start_time;
  log_type = le.log_type;
  msg_type = le.msg_type;
  notification_cmd = le.notification_cmd;
  notification_contact = le.notification_contact;
  output = le.output;
  retry = le.retry;
  service_description = le.service_description;
  service_id = le.service_id;
  status = le.status;
  return ;
}
