/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/notification/objects/defines.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
node_state::node_state() : value(ok) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(state_type type) : value(type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(short val) : value((state_type)val) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(int val) : value((state_type)val) {}

/**
 *  Copy constructor.
 *
 *  @param[in] type  The value to copy.
 */
node_state::node_state(node_state const& st) : value(st.value) {}

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
