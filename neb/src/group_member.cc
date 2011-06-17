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

#include "com/centreon/broker/neb/group_member.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy data members.
 *
 *  This method is used by the copy constructor and the assignment
 *  operator.
 *
 *  @param[in] gm Object to copy.
 */
void neb::group_member::_internal_copy(group_member const& gm) {
  group = gm.group;
  host_id = gm.host_id;
  instance_id = gm.instance_id;
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
neb::group_member::group_member() : host_id(0), instance_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] gm Object to copy.
 */
neb::group_member::group_member(group_member const& gm)
  : io::data(gm) {
  _internal_copy(gm);
}

/**
 *  Destructor.
 */
neb::group_member::~group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] gm Object to copy.
 *
 *  @return This object.
 */
neb::group_member& neb::group_member::operator=(group_member const& gm) {
  io::data::operator=(gm);
  _internal_copy(gm);
  return (*this);
}
