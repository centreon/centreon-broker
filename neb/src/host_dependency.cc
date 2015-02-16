/*
** Copyright 2009-2013,2015 Merethis
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
#include "com/centreon/broker/neb/host_dependency.hh"
#include "com/centreon/broker/neb/internal.hh"

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
host_dependency::host_dependency() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
host_dependency::host_dependency(host_dependency const& other)
  : dependency(other) {}

/**
 *  Destructor.
 */
host_dependency::~host_dependency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_dependency& host_dependency::operator=(host_dependency const& other) {
  dependency::operator=(other);
  return (*this);
}

/**
 *  Get the type of this object.
 *
 *  @return The event type.
 */
unsigned int host_dependency::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_dependency>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const host_dependency::entries[] = {
  mapping::entry(
    &host_dependency::dependency_period,
    "dependency_period",
    1),
  mapping::entry(
    &host_dependency::dependent_host_id,
    "dependent_host_id",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host_dependency::enabled,
    "",
    3),
  mapping::entry(
    &host_dependency::execution_failure_options,
    "execution_failure_options",
    4),
  mapping::entry(
    &host_dependency::inherits_parent,
    "inherits_parent",
    5),
  mapping::entry(
    &host_dependency::notification_failure_options,
    "notification_failure_options",
    6),
  mapping::entry(
    &host_dependency::host_id,
    "host_id",
    7,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host_dependency::instance_id,
    "",
    8),
  mapping::entry()
};

// Operations.
static io::data* new_host_dep() {
  return (new host_dependency);
}
io::event_info::event_operations const host_dependency::operations = {
  &new_host_dep
};
