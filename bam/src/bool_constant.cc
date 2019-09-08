/*
** Copyright 2016 Centreon
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

#include "com/centreon/broker/bam/bool_constant.hh"

using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] val  The constant value to assign.
 */
bool_constant::bool_constant(double val) : _value(val) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_constant::bool_constant(bool_constant const& right) : bool_value(right) {
  _value = right._value;
}

/**
 *  Destructor.
 */
bool_constant::~bool_constant() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_constant& bool_constant::operator=(bool_constant const& right) {
  bool_value::operator=(right);
  if (this != &right) {
    _value = right._value;
  }
  return (*this);
}

/**
 *  Get notified of child update.
 *
 *  @param[in] child    The child.
 *  @param[in] visitor  A visitor.
 *
 *  @return True if the parent was modified.
 */
bool bool_constant::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return (true);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
double bool_constant::value_hard() {
  return (_value);
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
double bool_constant::value_soft() {
  return (_value);
}

/**
 *  Is the state known ?
 *
 *  @return  True if the state is known.
 */
bool bool_constant::state_known() const {
  return (true);
}
