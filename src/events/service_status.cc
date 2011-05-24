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

#include "events/service_status.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members defined within service_status to
 *  the current instance. This method is used by the copy constructor
 *  and the assignment operator.
 *
 *  @param[in] ss Object to copy.
 */
void service_status::_internal_copy(service_status const& ss) {
  last_time_critical = ss.last_time_critical;
  last_time_ok = ss.last_time_ok;
  last_time_unknown = ss.last_time_unknown;
  last_time_warning = ss.last_time_warning;
  service_id = ss.service_id;
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
service_status::service_status()
  : last_time_critical(0),
    last_time_ok(0),
    last_time_unknown(0),
    last_time_warning(0),
    service_id(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given service_status to the current
 *  instance.
 *
 *  @param[in] ss Object to copy.
 */
service_status::service_status(service_status const& ss)
  : host_service_status(ss) {
  _internal_copy(ss);
}

/**
 *  Destructor.
 */
service_status::~service_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given service_status to the current
 *  instance.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_status& service_status::operator=(service_status const& ss) {
  host_service_status::operator=(ss);
  _internal_copy(ss);
  return (*this);
}

/**
 *  @brief Returns the type of the event (event::SERVICESTATUS).
 *
 *  This method is used to determine at runtime the type of event.
 *
 *  @return event::SERVICESTATUS
 */
int service_status::get_type() const {
  return (SERVICESTATUS);
}
