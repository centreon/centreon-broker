/*
** Copyright 2013,2015 Merethis
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

#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
instance_broadcast::instance_broadcast()
  : instance_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
instance_broadcast::instance_broadcast(instance_broadcast const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
instance_broadcast::~instance_broadcast() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
instance_broadcast& instance_broadcast::operator=(
                                          instance_broadcast const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int instance_broadcast::type() const {
  return (instance_broadcast::static_type());
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
void instance_broadcast::_internal_copy(instance_broadcast const& other) {
  instance_id = other.instance_id;
  instance_name = other.instance_name;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const instance_broadcast::entries[] = {
  mapping::entry(
    &instance_broadcast::instance_id,
    "instance_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &instance_broadcast::instance_name,
    "instance_name"),
  mapping::entry()
};

// Operations.
static io::data* new_instance_broadcast() {
  return (new instance_broadcast);
}
io::event_info::event_operations const instance_broadcast::operations = {
  &new_instance_broadcast
};
