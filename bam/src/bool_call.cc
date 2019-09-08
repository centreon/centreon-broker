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

#include "com/centreon/broker/bam/bool_call.hh"

using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the external expression.
 */
bool_call::bool_call(std::string const& name) : _name(name) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_call::bool_call(bool_call const& right) : bool_value(right) {
  _name = right._name;
  _expression = right._expression;
}

/**
 *  Destructor.
 */
bool_call::~bool_call() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_call& bool_call::operator=(bool_call const& right) {
  bool_value::operator=(right);
  if (this != &right) {
    _name = right._name;
    _expression = right._expression;
  }
  return (*this);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
double bool_call::value_hard() {
  if (!_expression)
    return 0;
  else
    return _expression->value_hard();
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
double bool_call::value_soft() {
  if (!_expression)
    return 0;
  else
    return _expression->value_hard();
}

/**
 *  Is the state known?
 *
 *  @return  True if the state is known.
 */
bool bool_call::state_known() const {
  if (!_expression)
    return false;
  else
    return _expression->state_known();
}

/**
 *  Get the name of this boolean expression.
 *
 *  @return  The name of this boolean expression.
 */
std::string const& bool_call::get_name() const {
  return (_name);
}

/**
 *  Set expression.
 *
 *  @param[in] expression  The expression.
 */
void bool_call::set_expression(std::shared_ptr<bool_value> expression) {
  _expression = expression;
}

/**
 *  Called when a child has update.
 *
 *  @param[in] child    The child.
 *  @param[in] visitor  The visitor.
 *
 *  @return  True if this was modified.
 */
bool bool_call::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return (true);
}
