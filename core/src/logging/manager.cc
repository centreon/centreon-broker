/*
** Copyright 2009-2011 Centreon
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

#include "com/centreon/broker/logging/manager.hh"
#include <cstdlib>
#include <cstring>

using namespace com::centreon::broker::logging;

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
manager::manager() : backend(), _limits{} {}

/**
 *  Compute optimizations to avoid as much log message generation as
 *  possible.
 */
void manager::_compute_optimizations() {
  memset(_limits, 0, sizeof(_limits));
  for (std::vector<manager_backend>::const_iterator it = _backends.begin(),
                                                    end = _backends.end();
       it != end; ++it)
    for (uint32_t i = 1; i <= static_cast<uint32_t>(it->l); ++i)
      _limits[i] |= it->types;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Get temporary logging object.
 *
 *  @param[in] t Log type.
 *  @param[in] l Log level.
 *
 *  @return Temporary logging object.
 */
temp_logger manager::get_temp_logger(type t, level l) noexcept {
  return temp_logger(t, l, (_limits[l] & t));
}

/**
 *  Get class instance.
 *
 *  @return Class instance.
 */
manager& manager::instance() {
  static manager instance;
  return instance;
}

/**
 *  Log message to backends.
 *
 *  @param[in] msg Message to log.
 *  @param[in] len Message length.
 *  @param[in] t   Log type.
 *  @param[in] l   Log level.
 */
void manager::log_msg(const char* msg, uint32_t len, type t, level l) noexcept {
  std::lock_guard<std::mutex> lock(_backendsm);
  for (std::vector<manager_backend>::iterator it = _backends.begin(),
                                              end = _backends.end();
       it != end; ++it)
    if (msg && (it->types & t) && (it->l >= l))
      it->b->log_msg(msg, len, t, l);
}

/**
 *  @brief Add or remove a log backend.
 *
 *  If either types or min_priority is 0, the backend will be removed.
 *
 *  @param[in] b            Backend.
 *  @param[in] types        Log types to store on this backend. Bitwise
 *                          OR of multiple logging::type.
 *  @param[in] min_priority Minimal priority of messages to be logged.
 */
void manager::log_on(std::shared_ptr<backend> b,
                     uint32_t types,
                     level min_priority) {
  std::lock_guard<std::mutex> lock(_backendsm);

  // Either add backend to list.
  if (types && min_priority) {
    manager_backend p;
    p.b = b;
    p.l = min_priority;
    p.types = types;
    _backends.push_back(p);
    for (uint32_t i = 1; i <= static_cast<uint32_t>(min_priority); ++i)
      _limits[i] |= types;
  }
  // Or remove it.
  else {
    for (std::vector<manager_backend>::iterator it = _backends.begin();
         it != _backends.end();)
      if (it->b == b)
        it = _backends.erase(it);
      else
        ++it;
    _compute_optimizations();
  }
}
