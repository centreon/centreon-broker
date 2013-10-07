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
#include "com/centreon/broker/neb/service_group.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
service_group::service_group() {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the service group object to the current
 *  instance.
 *
 *  @param[in] sg Object to copy.
 */
service_group::service_group(service_group const& sg) : group(sg) {}

/**
 *  Destructor.
 */
service_group::~service_group() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the service group object to the current
 *  instance.
 *
 *  @param[in] sg Object to copy.
 *
 *  @return This object.
 */
service_group& service_group::operator=(service_group const& sg) {
  group::operator=(sg);
  return (*this);
}

/**
 *  Get the event's type.
 *
 *  @return The event_type.
 */
unsigned int service_group::type() const {
  return (io::events::data_type<io::events::neb, neb::de_service_group>::value);
}
