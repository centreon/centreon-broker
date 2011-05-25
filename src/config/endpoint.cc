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

#include "config/endpoint.hh"

using namespace com::centreon::broker::config;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy data members.
 *
 *  Copy all data members from the given object to the current instance.
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] e Object to copy.
 */
void endpoint::_internal_copy(endpoint const& e) {
  failover = e.failover;
  if (!e.failover_config.isNull())
    failover_config.reset(new endpoint(*e.failover_config));
  else
    failover_config.reset();
  name = e.name;
  params = e.params;
  type = e.type;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
endpoint::endpoint() {}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
endpoint::endpoint(endpoint const& i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
endpoint& endpoint::operator=(endpoint const& i) {
  _internal_copy(i);
  return (*this);
}
