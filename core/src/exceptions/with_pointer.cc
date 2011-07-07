/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/exceptions/with_pointer.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] base Base exception.
 *  @param[in] ptr  Data pointer.
 */
with_pointer::with_pointer(msg const& base, QSharedPointer<io::data> ptr)
  : _base(base.clone()), _ptr(ptr) {}

/**
 *  Copy constructor.
 *
 *  @param[in] wp Object to copy.
 */
with_pointer::with_pointer(with_pointer const& e)
  : msg(e), _base(e._base->clone()), _ptr(e._ptr) {}

/**
 *  Destructor.
 */
with_pointer::~with_pointer() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] wp Object to copy.
 *
 *  @return This object.
 */
with_pointer& with_pointer::operator=(with_pointer const& wp) {
  _base.reset(wp._base->clone());
  _ptr = wp._ptr;
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return Copy of this object.
 */
msg* with_pointer::clone() const {
  return (new with_pointer(*this));
}

/**
 *  Get the pointer associated with the exception.
 *
 *  @return Pointer associated with this exception.
 */
QSharedPointer<io::data> with_pointer::ptr() const {
  return (_ptr);
}

/**
 *  Rethrow the subexception.
 */
void with_pointer::rethrow() {
  _base->rethrow();
  return ;
}
