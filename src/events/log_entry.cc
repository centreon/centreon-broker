/*
** Copyright 2009-2011 MERETHIS
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

#include "events/log_entry.hh"

using namespace com::centreon::broker::events;

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
  instance_name = le.instance_name;
  issue_start_time = le.issue_start_time;
  msg_type = le.msg_type;
  notification_cmd = le.notification_cmd;
  notification_contact = le.notification_contact;
  output = le.output;
  retry = le.retry;
  service_description = le.service_description;
  service_id = le.service_id;
  status = le.status;
  type = le.type;
  return ;
}

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
    issue_start_time(0),
    msg_type(0),
    retry(0),
    service_id(0),
    status(0),
    type(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] le Object to copy.
 */
log_entry::log_entry(log_entry const& le) : event(le) {
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
  event::operator=(le);
  _internal_copy(le);
  return (*this);
}

/**
 *  @brief Returns the type of the event (event::LOG).
 *
 *  This method can be useful for runtime event type identification.
 *
 *  @return event::LOG
 */
int log_entry::get_type() const {
  return (event::LOG);
}
