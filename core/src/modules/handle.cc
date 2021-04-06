/*
** Copyright 2011-2013,2015, 2021 Centreon
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

#include "com/centreon/broker/modules/handle.hh"
#include <cstring>
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::modules;

// Routine symbols.
char const* handle::deinitialization("broker_module_deinit");
char const* handle::initialization("broker_module_init");
char const* handle::updatization("broker_module_update");
char const* handle::versionning("broker_module_version");

handle::handle(const std::string& filename, const void* arg)
    : _filename{filename},
      _handle{dlopen(_filename.c_str(), RTLD_NOW | RTLD_GLOBAL)} {
  // Could not load library.
  if (!_handle)
    throw msg_fmt("modules: could not load library '{}': {}", _filename,
                  dlerror());
  // Initialize module.
  _check_version();
  _init(arg);
}

/**
 *  Destructor.
 */
handle::~handle() noexcept {
  try {
    _close();
  } catch (std::exception const& e) {
    logging::error(logging::high) << e.what();
  } catch (...) {
    logging::error(logging::high) << "modules: unknown error while "
                                     "unloading '"
                                  << _filename << "'";
  }
}

/**
 *  @brief Close the library object.
 *
 *  If the underlying library object is open, this method will call the
 *  module's deinitialization routine (if it exists) and then unload the
 *  library.
 */
void handle::_close() {
  if (is_open()) {
    // Log message.
    log_v2::core()->info("modules: closing '{}'", _filename);

    // Find deinitialization routine.
    union {
      void (*code)();
      void* data;
    } sym;
    sym.data = dlsym(_handle, deinitialization);

    // Could not find deinitialization routine.
    char const* error_str{dlerror()};
    if (error_str) {
      log_v2::core()->info(
          "modules: could not find deinitialization routine in '{}': {}",
          _filename, error_str);
    }
    // Call deinitialization routine.
    else {
      log_v2::core()->debug("modules: running deinitialization routine of '{}'",
                            _filename);
      (*(sym.code))();
    }

    // Reset library handle.
    log_v2::core()->debug("modules: unloading library '{}'", _filename);
    // Library was not unloaded.
    if (dlclose(_handle)) {
      char const* error_str{dlerror()};
      log_v2::core()->info("modules: could not unload library '{}': {}",
                           _filename, error_str);
    } else
      _handle = nullptr;
  }
}

/**
 *  Check if the library is loaded.
 *
 *  @return true if the library is loaded, false otherwise.
 */
bool handle::is_open() const {
  return _handle;
}

/**
 *  Update a library file.
 *
 *  @param[in] arg Library argument.
 */
void handle::update(void const* arg) {
  // Check that library is loaded.
  if (!is_open())
    throw msg_fmt("modules: could not update module that is not loaded");

  // Find update routine.
  union {
    void (*code)();
    void* data;
  } sym;
  sym.data = dlsym(_handle, updatization);

  // Found routine.
  if (sym.data) {
    log_v2::core()->debug("modules: running update routine of '{}'", _filename);
    (*(void (*)(void const*))(sym.code))(arg);
  }
}

/**
 *  Check that the module has the correct version.
 */
void handle::_check_version() {
  // Find version symbol.
  log_v2::core()->debug("modules: checking module version (symbol {}) in '{}'",
                        versionning, _filename);

  char const** version = (char const**)dlsym(_handle, versionning);

  // Could not find version symbol.
  if (!version) {
    char const* error_str{dlerror()};
    throw msg_fmt(
        "modules: could not find version in '{}'"
        " (not a Centreon Broker module ?): {}",
        _filename, error_str);
  }
  if (!*version)
    throw msg_fmt(
        "modules: version symbol of module '{}'"
        " is empty (not a Centreon Broker module ?)",
        _filename);

  // Check version.
  if (::strcmp(CENTREON_BROKER_VERSION, *version) != 0)
    throw msg_fmt(
        "modules: version mismatch in '{}': exepected '{}', found '{}'",
        _filename, CENTREON_BROKER_VERSION, *version);
}

/**
 *  Call the module's initialization routine.
 *
 *  @param[in] arg Module argument.
 */
void handle::_init(void const* arg) {
  // Find initialization routine.
  union {
    void (*code)();
    void* data;
  } sym;
  sym.data = dlsym(_handle, initialization);

  // Could not find initialization routine.
  if (!sym.data) {
    char const* error_str = dlerror();
    throw msg_fmt(
        "modules: could not find initialization routine in '{}' (not a "
        "Centreon Broker module ?): {}",
        _filename, error_str);
  }

  // Call initialization routine.
  log_v2::core()->debug("modules: running initialization routine of '{}'",
                        _filename);
  (*(void (*)(void const*))(sym.code))(arg);
}
