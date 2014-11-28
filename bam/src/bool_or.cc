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

#include "com/centreon/broker/bam/bool_or.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_or::bool_or() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_or::bool_or(bool_or const& right)
  : bool_binary_operator(right) {}

/**
 *  Destructor.
 */
bool_or::~bool_or() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_or& bool_or::operator=(bool_or const& right) {
  bool_binary_operator::operator=(right);
  return (*this);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
bool bool_or::value_hard() {
  return (_left_hard || _right_hard);
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
bool bool_or::value_soft() {
  return (_left_soft || _right_soft);
}
