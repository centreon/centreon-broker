/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/objects/defines.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
node_state::node_state()
  : value(ok) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(state_type type)
  : value(type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(short val)
  : value((state_type)val) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(int val)
  : value((state_type)val) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(node_state const& st)
  : value(st.value) {}

/**
 *  Assignment operator.
 *
 *  @param[in] type  The value to copy.
 *
 *  @return          A reference to this object.
 */
node_state& node_state::operator=(node_state const& st) {
  if (this != &st)
    value = st.value;
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] st  The value to test for equality.
 *
 *  @return        True if equal.
 */
bool node_state::operator==(node_state const& st) const throw() {
  return (value == st.value);
}

/**
 *  Equality operator.
 *
 *  @param[in] st  The value to test for equality.
 *
 *  @return        True if equal.
 */
bool node_state::operator==(node_state::state_type type) const throw() {
  return (value == type);
}

/**
 *  Cast operator.
 *
 *  @return  The value casted to int.
 */
node_state::operator int() const throw() {
  return ((int)(value));
}

