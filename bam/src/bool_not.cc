/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/bam/bool_not.hh"

using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] val Value that will be negated.
 */
bool_not::bool_not(bool_value::ptr val) : _value(val) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_not::bool_not(bool_not const& right) : bool_value(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
bool_not::~bool_not() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_not& bool_not::operator=(bool_not const& right) {
  if (this != &right) {
    bool_value::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  @brief Notify of the change of value of the child.
 *
 *  This class does not cache values. This method is therefore useless
 *  for this class.
 *
 *  @param[in] child     Unused.
 *  @param[out] visitor  Unused.
 *
 *  @return              True;
 */
bool bool_not::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return (true);
}

/**
 *  Set value object.
 *
 *  @param[in] value Value object whose value will be negated.
 */
void bool_not::set_value(std::shared_ptr<bool_value>& value) {
  _value = value;
  return;
}

/**
 *  Get the hard value.
 *
 *  @return Hard value.
 */
double bool_not::value_hard() {
  return (!_value->value_hard());
}

/**
 *  Get the soft value.
 *
 *  @return Soft value.
 */
double bool_not::value_soft() {
  return (!_value->value_soft());
}

/**
 *  Copy the internal data members.
 *
 *  @param[in] right Object to copy.
 */
void bool_not::_internal_copy(bool_not const& right) {
  _value = right._value;
  return;
}

/**
 *  Get if the state is known, i.e has been computed at least once.
 *
 *  @return  True if the state is known.
 */
bool bool_not::state_known() const {
  return (_value && _value->state_known());
}

/**
 *  Is this expression in downtime?
 *
 *  @return  True if this expression is in downtime.
 */
bool bool_not::in_downtime() const {
  return (_value && _value->in_downtime());
}
