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

#include "com/centreon/broker/neb/host_state.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
host_state::host_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hs Object to copy.
 */
host_state::host_state(host_state const& hs) : state(hs) {}

/**
 *  Destructor.
 */
host_state::~host_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] hs Object to copy.
 *
 *  @return This instance.
 */
host_state& host_state::operator=(host_state const& hs) {
  state::operator=(hs);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The string "com::centreon::broker::neb::host_state".
 */
QString const& host_state::type() const {
  static QString const hs_type("com::centreon::broker::neb::host_state");
  return (hs_type);
}
