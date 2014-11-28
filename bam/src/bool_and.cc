/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/bool_and.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_and::bool_and() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_and::bool_and(bool_and const& right)
  : bool_binary_operator(right) {}

/**
 *  Destructor.
 */
bool_and::~bool_and() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_and& bool_and::operator=(bool_and const& right) {
  bool_binary_operator::operator=(right);
  return (*this);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
bool bool_and::value_hard() {
  return (_left_hard && _right_hard);
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
bool bool_and::value_soft() {
  return (_left_soft && _right_soft);
}
