/*
** Copyright 2011,2020 Centreon
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

#include "com/centreon/broker/multiplexing/hooker.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker::multiplexing;

/**
 *  Default constructor.
 */
hooker::hooker() : io::stream("hooker"), _registered{false} {}

/**
 *  Destructor.
 */
hooker::~hooker() noexcept {}

/**
 *  Enable or disable hooking.
 *
 *  @param[in] should_hook  Set to true if hooker should hook.
 */
void hooker::hook(bool should_hook) {
  if (_registered && !should_hook) {
    engine::instance().unhook(*this);
    _registered = false;
  } else if (!_registered && should_hook) {
    engine::instance().hook(*this);
    _registered = true;
  }
}
