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

#include "com/centreon/broker/bam/bool_binary_operator.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_binary_operator::bool_binary_operator() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_binary_operator::bool_binary_operator(
                        bool_binary_operator const& right)
  : bool_value(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
bool_binary_operator::~bool_binary_operator() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_binary_operator& bool_binary_operator::operator=(
                                              bool_binary_operator const& right) {
  if (this != &right) {
    bool_value::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Set left member.
 *
 *  @param[in] left Left member of the boolean operator.
 */
void bool_binary_operator::set_left(
                             misc::shared_ptr<bool_value>& left) {
  _left = left;
  return ;
}

/**
 *  Set right member.
 *
 *  @param[in] right Right member of the boolean operator.
 */
void bool_binary_operator::set_right(
                             misc::shared_ptr<bool_value>& right) {
  _right = right;
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void bool_binary_operator::_internal_copy(
                             bool_binary_operator const& right) {
  _left = right._left;
  _right = right._right;
  return ;
}
