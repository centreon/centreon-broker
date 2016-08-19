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

#include <cmath>
#include "com/centreon/broker/bam/bool_operation.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 *
 *  @param[in] op  The operation in string format.
 */
bool_operation::bool_operation(std::string const& op) {
  if (op == "+")
    _type = addition;
  else if (op == "-")
    _type = substraction;
  else if (op == "*")
    _type = multiplication;
  else if (op == "/")
    _type = division;
  else if (op == "%")
    _type = modulo;
  else
    _type = addition;
}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_operation::bool_operation(bool_operation const& right)
  : bool_binary_operator(right) {
  _type = right._type;
}

/**
 *  Destructor.
 */
bool_operation::~bool_operation() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_operation& bool_operation::operator=(bool_operation const& right) {
  bool_binary_operator::operator=(right);
  if (this != &right) {
    _type = right._type;
  }
  return (*this);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
double bool_operation::value_hard() {
  switch (_type) {
  case addition:
    return (_left_hard + _right_hard);
  case substraction:
    return (_left_hard - _right_hard);
  case multiplication:
    return (_left_hard * _right_hard);
  case division:
    if (_right_hard == 0)
      return (NAN);
    return (_left_hard / _right_hard);
  case modulo:
    if (_right_hard == 0)
      return (NAN);
    return (_left_hard % _right_hard);
  }
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
double bool_operation::value_soft() {
  switch (_type) {
  case addition:
    return (_left_soft + _right_soft);
  case substraction:
    return (_left_soft - _right_soft);
  case multiplication:
    return (_left_soft * _right_soft);
  case division:
    if (_right_soft == 0)
      return (NAN);
    return (_left_soft / _right_soft);
  case modulo:
    if (_right_soft == 0)
      return (NAN);
    return (_left_soft % _right_soft);
  }
}

/**
 *  Is the state known?
 *
 *  @return  True if the state is known.
 */
bool bool_operation::state_known() const {
  bool known = bool_binary_operator::state_known();
  if (known
      && (_type == division || _type == modulo)
      && (_right_hard == 0 || _right_soft == 0))
    return (false);
  else
    return (known);
}

