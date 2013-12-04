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
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  Initialize all members to 0, NULL or equivalent.
 */
host_status::host_status()
  : last_time_down(0),
    last_time_unreachable(0),
    last_time_up(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] hs Object to copy.
 */
host_status::host_status(host_status const& hs)
  : host_service_status(hs) {
  _internal_copy(hs);
}

/**
 *  Destructor.
 */
host_status::~host_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] hs Object to copy.
 *
 *  @return This object.
 */
host_status& host_status::operator=(host_status const& hs) {
  host_service_status::operator=(hs);
  _internal_copy(hs);
  return (*this);
}

/**
 *  Returns the type of the event.
 *
 *  @return The event_type.
 */
unsigned int host_status::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy all internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members of HostStatus to the current
 *  instance. This method is use by the copy constructor and the
 *  assignment operator.
 *
 *  @param[in] hs Object to copy.
 */
void host_status::_internal_copy(host_status const& hs) {
  last_time_down = hs.last_time_down;
  last_time_unreachable = hs.last_time_unreachable;
  last_time_up = hs.last_time_up;
  return ;
}
