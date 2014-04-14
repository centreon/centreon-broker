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

#include "com/centreon/broker/bam/bool_not.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_not::bool_not() {}

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
 *  @param[in] child Child.
 */
void bool_not::child_has_update(misc::shared_ptr<computable>& child) {
  (void)child;
  return ;
}

/**
 *  Set value object.
 *
 *  @param[in] value Value object whose value will be negated.
 */
void bool_not::set_value(misc::shared_ptr<bool_value>& value) {
  _value = value;
  return ;
}

/**
 *  Get the hard value.
 *
 *  @return Hard value.
 */
bool bool_not::value_hard() {
  return (!_value->value_hard());
}

/**
 *  Get the soft value.
 *
 *  @return Soft value.
 */
bool bool_not::value_soft() {
  return (!_value->value_soft());
}

/**
 *  Copy the internal data members.
 *
 *  @param[in] right Object to copy.
 */
void bool_not::_internal_copy(bool_not const& right) {
  _value = right._value;
  return ;
}
