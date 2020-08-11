/*
** Copyright 2012 Centreon
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

#include "com/centreon/broker/neb/callback.hh"

#include <cstdlib>

#include "com/centreon/broker/exceptions/msg.hh"
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
callback::callback(int id, void* handle, int (*function)(int, void*))
    : _function(function), _id(id) {
  if (neb_register_callback(_id, handle, 0, _function) != OK)
    throw(exceptions::msg()
          << "callbacks: registration of callback " << id << " failed");
}

/**
 *  Destructor.
 */
callback::~callback() noexcept {
  neb_deregister_callback(_id, _function);
}
