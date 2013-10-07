/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/neb/service_dependency.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_dependency::service_dependency()
  : dependent_service_id(0), service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] sd Object to copy.
 */
service_dependency::service_dependency(service_dependency const& sd)
  : dependency(sd) {
  _internal_copy(sd);
}

/**
 *  Destructor.
 */
service_dependency::~service_dependency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] sd Object to copy.
 *
 *  @return This object.
 */
service_dependency& service_dependency::operator=(
                                          service_dependency const& sd) {
  if (this != &sd) {
    dependency::operator=(sd);
    _internal_copy(sd);
  }
  return (*this);
}

/**
 *  Get the type of this object.
 *
 *  @return The string "com::centreon::broker::neb::service_dependency".
 */
unsigned int service_dependency::type() const {
  return (io::data::data_type(io::data::neb, neb::service_dependency));
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal members from the given object.
 *
 *  @param[in] sd Object to copy.
 */
void service_dependency::_internal_copy(service_dependency const& sd) {
  dependent_service_id = sd.dependent_service_id;
  service_id = sd.service_id;
  return ;
}
