/*
** Copyright 2009-2011 Merethis
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

#include "events/service_state.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_state::service_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
service_state::service_state(service_state const& ss) : state(ss) {}

/**
 *  Destructor.
 */
service_state::~service_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_state& service_state::operator=(service_state const& ss) {
  state::operator=(ss);
  return (*this);
}

/**
 *  Get the type of this object (SERVICESTATE).
 *
 *  @return event::SERVICESTATE.
 */
unsigned int service_state::type() const {
  return (SERVICESTATE);
}
