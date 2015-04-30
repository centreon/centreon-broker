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
#include "com/centreon/broker/neb/custom_variable_status.hh"
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
custom_variable_status::custom_variable_status()
  : host_id(0),
    modified(true),
    service_id(0),
    update_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
custom_variable_status::custom_variable_status(
                          custom_variable_status const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
custom_variable_status::~custom_variable_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
custom_variable_status& custom_variable_status::operator=(
                          custom_variable_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int custom_variable_status::type() const {
  return (custom_variable_status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int custom_variable_status::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_custom_variable_status>::value);
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
void custom_variable_status::_internal_copy(custom_variable_status const& other) {
  host_id = other.host_id;
  modified = other.modified;
  name = other.name;
  service_id = other.service_id;
  update_time = other.update_time;
  value = other.value;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const custom_variable_status::entries[] = {
  mapping::entry(
    &custom_variable_status::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &custom_variable_status::modified,
    "modified"),
  mapping::entry(
    &custom_variable_status::name,
    "name"),
  mapping::entry(
    &custom_variable_status::service_id,
    "service_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &custom_variable_status::update_time,
    "update_time",
    mapping::entry::invalid_on_minus_one),
  mapping::entry(
    &custom_variable_status::value,
    "value"),
  mapping::entry()
};

// Operations.
static io::data* new_custom_var_status() {
  return (new custom_variable_status);
}
io::event_info::event_operations const custom_variable_status::operations = {
  &new_custom_var_status
};
