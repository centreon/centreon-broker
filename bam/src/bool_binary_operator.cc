/*
** Copyright 2014-2016 Centreon
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

#include "com/centreon/broker/bam/bool_binary_operator.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_binary_operator::bool_binary_operator()
  : _left_hard(false),
    _left_soft(false),
    _right_hard(false),
    _right_soft(false),
    _state_known(false),
    _in_downtime(false) {}

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
                             io::stream* visitor) {
  (void)visitor;
  bool retval(true);

  // Check operation members values.
  if (child) {
    if (child == _left.data()) {
      bool value_hard(_left->value_hard());
      bool value_soft(_left->value_soft());
      if ((_left_hard != value_hard) || (_left_soft != value_soft)) {
        _left_hard = value_hard;
        _left_soft = value_soft;
        retval = true;
      }
    }
    else if (child == _right.data()) {
      bool value_hard(_right->value_hard());
      bool value_soft(_right->value_soft());
      if ((_right_hard != value_hard) || (_right_soft == value_soft)) {
        _right_hard = value_hard;
        _right_soft = value_soft;
        retval = true;
      }
    }
  }

  // Check known flag.
  bool known(state_known());
  if (_state_known != known) {
    _state_known = known;
    retval = true;
  }

  // Check downtime flag.
  bool in_dt(in_downtime());
  if (_in_downtime != in_dt) {
    _in_downtime = in_dt;
    retval = true;
  }

  return (retval);
}

/**
 *  Set left member.
 *
 *  @param[in] left Left member of the boolean operator.
 */
void bool_binary_operator::set_left(
                             misc::shared_ptr<bool_value> const& left) {
  _left = left;
  return ;
}

/**
 *  Set right member.
 *
 *  @param[in] right Right member of the boolean operator.
 */
void bool_binary_operator::set_right(
                             misc::shared_ptr<bool_value> const& right) {
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
  _state_known = right._state_known;
  _in_downtime = right._in_downtime;
  return ;
}

/**
 *  Get if the state is known, i.e has been computed at least once.
 *
 *  @return  True if the state is known.
 */
bool bool_binary_operator::state_known() const {
  return (!_left.isNull()
          && !_right.isNull()
          && _left->state_known()
          && _right->state_known());
}

/**
 *  Is this expression in downtime?
 *
 *  @return  True if this expression is in downtime.
 */
bool bool_binary_operator::in_downtime() const {
  return ((!_left.isNull() && _left->in_downtime())
          || (!_right.isNull() && _right->in_downtime()));
}
