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
bool_binary_operator::bool_binary_operator()
  : _left_hard(false),
    _left_soft(false),
    _right_hard(false),
    _right_soft(false) {}

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
 *  Notification of child update.
 *
 *  @param[in] child     Child that got updated.
 *  @param[out] visitor  Visitor.
 *
 *  @return              True if the values of this object were modified.
 */
bool bool_binary_operator::child_has_update(
                             computable* child,
                             stream* visitor) {
  (void)visitor;
  if (child) {
    if (child == _left.data()) {
      if (_left_hard == _left->value_hard()
            && _left_soft == _left->value_soft())
        return false;
      _left_hard = _left->value_hard();
      _left_soft = _left->value_soft();
    }
    else if (child == _right.data()) {
      if (_right_hard == _right->value_hard()
            && _right_soft == _right->value_soft())
        return false;
      _right_hard = _right->value_hard();
      _right_soft = _right->value_soft();
    }
  }
  return true;
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
  _left_hard = right._left_hard;
  _left_soft = right._left_soft;
  _right = right._right;
  _right_hard = right._right_hard;
  _right_soft = right._right_soft;
  return ;
}
