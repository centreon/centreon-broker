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
#include "com/centreon/broker/neb/service_dependency.hh"

using namespace com::centreon::broker;
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
 *  @return The event_type.
 */
unsigned int service_dependency::type() const {
  return (service_dependency::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int service_dependency::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_service_dependency>::value);
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

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const service_dependency::entries[] = {
  mapping::entry(
    &service_dependency::dependency_period,
    "dependency_period",
    1),
  mapping::entry(
    &service_dependency::dependent_host_id,
    "dependent_host_id",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_dependency::dependent_service_id,
    "dependent_service_id",
    3,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_dependency::enabled,
    "",
    4),
  mapping::entry(
    &service_dependency::execution_failure_options,
    "execution_failure_options",
    5),
  mapping::entry(
    &service_dependency::host_id,
    "host_id",
    6,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_dependency::inherits_parent,
    "inherits_parent",
    7),
  mapping::entry(
    &service_dependency::service_id,
    "service_id",
    8,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &service_dependency::instance_id,
    "",
    9),
  mapping::entry()
};

// Operations.
static io::data* new_service_dependency() {
  return (new service_dependency);
}
io::event_info::event_operations const service_dependency::operations = {
  &new_service_dependency
};
