/*
** Copyright 2015 Merethis
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

#include <cmath>
#include "com/centreon/broker/dumper/entries/boolean.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
boolean::boolean()
  : enable(true),
    poller_id(0),
    boolean_id(0),
    bool_state(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
boolean::boolean(boolean const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
boolean::~boolean() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
boolean& boolean::operator=(boolean const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are equal.
 */
bool boolean::operator==(boolean const& other) const {
  return ((enable == other.enable)
          && (poller_id == other.poller_id)
          && (boolean_id == other.boolean_id)
          && (bool_state == other.bool_state)
          && (name == other.name)
          && (expression == other.expression)
          && (comment == other.comment));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are not equal.
 */
bool boolean::operator!=(boolean const& other) const {
  return (!operator==(other));
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int boolean::type() const {
  return (io::events::data_type<io::events::dumper, dumper::de_entries_boolean>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data objects.
 *
 *  @param[in] other  Object to copy.
 */
void boolean::_internal_copy(boolean const& other) {
  enable = other.enable;
  poller_id = other.poller_id;
  boolean_id = other.boolean_id;
  name = other.name;
  expression = other.expression;
  bool_state = other.bool_state;
  comment = other.comment;
  return ;
}
