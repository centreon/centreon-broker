/*
** Copyright 2011-2012 Merethis
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
with_pointer::with_pointer(
                msg const& base,
                misc::shared_ptr<io::data> ptr) : _ptr(ptr) {
  try {
    _base.reset(base.clone());
  }
  catch (...) {
    _base.reset();
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] wp Object to copy.
 */
with_pointer::with_pointer(with_pointer const& e)
  : msg(e), _ptr(e._ptr) {
  if (e._base.get()) {
    try {
      _base.reset(e._base->clone());
    }
    catch (...) {
      _base.reset();
    }
  }
}

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
  if (!wp._base.get())
    _base.reset();
  else {
    try {
      _base.reset(wp._base->clone());
    }
    catch (...) {
      _base.reset();
    }
  }
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
misc::shared_ptr<io::data> with_pointer::ptr() const {
  return (_ptr);
}

/**
 *  Rethrow the subexception.
 */
void with_pointer::rethrow() const {
  _base->rethrow();
  return ;
}

/**
 *  Return exception message.
 *
 *  @return Exception message.
 */
char const* with_pointer::what() const throw () {
  return (!_base.get()
          ? "generic: unknown chained exception"
          : _base->what());
}
