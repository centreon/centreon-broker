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
**
** For more information: contact@centreon.com
*/

#include "events/service_group.hh"

using namespace events;

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
 *  @brief Get the event's type.
 *
 *  Returns the type of this event (event::SERVICEGROUP). This can be
 *  useful for runtime event type identification.
 *
 *  @return event::SERVICEGROUP
 */
int service_group::get_type() const {
  return (event::SERVICEGROUP);
}
