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
#include "com/centreon/broker/neb/host_group.hh"
#include "com/centreon/broker/neb/internal.hh"

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
host_group::host_group() {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the host group object to the current instance.
 *
 *  @param[in] hg Object to copy.
 */
host_group::host_group(host_group const& hg) : group(hg) {}

/**
 *  Destructor.
 */
host_group::~host_group() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the host group object to the current instance.
 *
 *  @param[in] hg Object to copy.
 *
 *  @return This object.
 */
host_group& host_group::operator=(host_group const& hg) {
  group::operator=(hg);
  return (*this);
}

/**
 *  Get the event's type.
 *
 *  @return The event type.
 */
unsigned int host_group::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_group>::value);
}
