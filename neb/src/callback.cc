/*
** Copyright 2012 Merethis
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

#include <assert.h>
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/callback.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/nebcallbacks.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 *
 *  @param[in] id       Callback ID.
 *  @param[in] handle   Module handle.
 *  @param[in] function Callback function.
 */
callback::callback(int id, void* handle, int (* function)(int, void*))
  : _function(function), _id(id) {
  if (neb_register_callback(_id, handle, 0, _function) != OK)
    throw (exceptions::msg() << "callbacks: registration of callback "
           << id << " failed");
}

/**
 *  Destructor.
 */
callback::~callback() throw () {
  neb_deregister_callback(_id, _function);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
callback::callback(callback const& right) {
  _internal_copy(right);
}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
callback& callback::operator=(callback const& right) {
  _internal_copy(right);
  return (*this);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Unused.
 */
void callback::_internal_copy(callback const& right) {
  (void)right;
  assert(!"callback is not copyable");
  abort();
  return ;
}
