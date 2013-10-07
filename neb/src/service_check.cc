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
#include "com/centreon/broker/neb/service_check.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_check::service_check() : service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] service_check Object to copy.
 */
service_check::service_check(service_check const& sc)
  : check(sc), service_id(sc.service_id) {}

/**
 *  Destructor.
 */
service_check::~service_check() {}

/**
 *  Assignment operator.
 *
 *  @param[in] sc Object to copy.
 *
 *  @return This object.
 */
service_check& service_check::operator=(service_check const& sc) {
  check::operator=(sc);
  service_id = sc.service_id;
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int service_check::type() const {
  return (io::events::data_type<io::events::neb, neb::de_service_check>::value);
}
