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

#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_group_member.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_group_member::service_group_member() : service_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] sgm Object to copy.
 */
service_group_member::service_group_member(service_group_member const& sgm)
  : group_member(sgm), service_id(sgm.service_id) {}

/**
 *  Destructor.
 */
service_group_member::~service_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] sgm Object to copy.
 *
 *  @return This object.
 */
service_group_member& service_group_member::operator=(service_group_member const& sgm) {
  group_member::operator=(sgm);
  service_id = sgm.service_id;
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int service_group_member::type() const {
  return (io::data::data_type(io::data::neb, neb::de_service_group_member));
}
