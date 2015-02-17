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
#include "com/centreon/broker/neb/custom_variable.hh"
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
custom_variable::custom_variable() : var_type(0) {
  modified = false;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
custom_variable::custom_variable(custom_variable const& other)
  : custom_variable_status(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
custom_variable::~custom_variable() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
custom_variable& custom_variable::operator=(custom_variable const& other) {
  if (this != &other) {
    custom_variable_status::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int custom_variable::type() const {
  return (custom_variable::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int custom_variable::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_custom_variable>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void custom_variable::_internal_copy(custom_variable const& other) {
  var_type = other.var_type;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const custom_variable::entries[] = {
  mapping::entry(
    &custom_variable::host_id,
    "host_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &custom_variable::modified,
    "modified",
    2),
  mapping::entry(
    &custom_variable::name,
    "name",
    3),
  mapping::entry(
    &custom_variable::service_id,
    "service_id",
    4,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &custom_variable::update_time,
    "update_time",
    5),
  mapping::entry(
    &custom_variable::var_type,
    "type",
    6),
  mapping::entry(
    &custom_variable::value,
    "value",
    7),
  mapping::entry(
    &custom_variable::value,
    "default_value",
    8),
  mapping::entry(
    &custom_variable::instance_id,
    "",
    9),
  mapping::entry()
};

// Operations.
static io::data* new_custom_var() {
  return (new custom_variable);
}
io::event_info::event_operations const custom_variable::operations = {
  &new_custom_var
};
