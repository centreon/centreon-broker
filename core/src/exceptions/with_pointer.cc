/*
** Copyright 2011-2013 Centreon
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
