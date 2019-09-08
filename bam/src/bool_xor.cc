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

#include "com/centreon/broker/bam/bool_xor.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_xor::bool_xor() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_xor::bool_xor(bool_xor const& right) : bool_binary_operator(right) {}

/**
 *  Destructor.
 */
bool_xor::~bool_xor() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_xor& bool_xor::operator=(bool_xor const& right) {
  bool_binary_operator::operator=(right);
  return (*this);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
double bool_xor::value_hard() {
  return ((!_left_hard && _right_hard) || (_left_hard && !_right_hard));
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
double bool_xor::value_soft() {
  return ((!_left_soft && _right_soft) || (_left_soft && !_right_soft));
}
