/*
** Copyright 2011-2013,2015 Centreon
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

#include <cstring>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/modules/handle.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::modules;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Routine symbols.
char const* handle::deinitialization("broker_module_deinit");
char const* handle::initialization("broker_module_init");
char const* handle::updatization("broker_module_update");
char const* handle::versionning("broker_module_version");

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
handle::handle() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
handle::handle(handle const& other) {
  this->open(other._handle.fileName().toStdString());
}

/**
 *  Destructor.
 */
handle::~handle() {
  try {
    this->close();
  }
  catch (std::exception const& e) {
    logging::error(logging::high) << e.what();
  }
  catch (...) {
    logging::error(logging::high) << "modules: unknown error while " \
      "unloading '" << _handle.fileName().toStdString() << "'";
  }
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
handle& handle::operator=(handle const& other) {
  this->close();
  this->open(other._handle.fileName().toStdString());
  return (*this);
}

/**
 *  @brief Close the library object.
 *
 *  If the underlying library object is open, this method will call the
 *  module's deinitialization routine (if it exists) and then unload the
 *  library.
 */
void handle::close() {
  if (is_open()) {
    // Log message.
    logging::info(logging::medium) << "modules: closing '"
      << _handle.fileName().toStdString() << "'";

    // Find deinitialization routine.
    union {
      void (* code)();
      void*   data;
    } sym;
    sym.data = _handle.resolve(deinitialization);

    // Could not find deinitialization routine.
    if (!sym.data) {
      QString error_str(_handle.errorString());
      logging::info(logging::medium) << "modules: could not find "
           "deinitialization routine in '" << _handle.fileName().toStdString()
        << "': " << error_str.toStdString();
    }
    // Call deinitialization routine.
    else {
      logging::debug(logging::low)
        << "modules: running deinitialization routine of '"
        << _handle.fileName().toStdString() << "'";
      (*(sym.code))();
    }

    // Reset library handle.
    logging::debug(logging::low) << "modules: unloading library '"
      << _handle.fileName().toStdString() << "'";
    // Library was not unloaded.
    if (!_handle.unload()) {
      QString error_str(_handle.errorString());
      logging::info(logging::medium)
        << "modules: could not unload library '"
        << _handle.fileName().toStdString() << "': " << error_str.toStdString();
    }
  }
  return ;
}

/**
 *  Check if the library is loaded.
 *
 *  @return true if the library is loaded, false otherwise.
 */
bool handle::is_open() const {
  return (_handle.isLoaded());
}

/**
 *  Load a library file.
 *
 *  @param[in] filename Library filename.
 *  @param[in] arg      Library argument.
 */
void handle::open(std::string const& filename, void const* arg) {
  // Close library if previously open.
  this->close();

  // Load library.
  logging::debug(logging::medium) << "modules: loading library '"
    << filename << "'";
  _handle.setFileName(filename.c_str());
  _handle.setLoadHints(QLibrary::ResolveAllSymbolsHint
    | QLibrary::ExportExternalSymbolsHint);

  // Could not load library.
  if (!_handle.load())
    throw (exceptions::msg() << "modules: could not load library '"
             << filename << "': " << _handle.errorString());

  // Initialize module.
  _check_version();
  _init(arg);

  return ;
}

/**
 *  Update a library file.
 *
 *  @param[in] arg Library argument.
 */
void handle::update(void const* arg) {
  // Check that library is loaded.
  if (!is_open())
    throw (exceptions::msg() << "modules: could not update "
                                "module that is not loaded");

  // Find update routine.
  union {
    void (* code)();
    void*   data;
  } sym;
  sym.data = _handle.resolve(updatization);

  // Found routine.
  if (sym.data) {
    logging::debug(logging::low)
      << "modules: running update routine of '"
      << _handle.fileName().toStdString() << "'";
    (*(void (*)(void const*))(sym.code))(arg);
  }

  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Check that the module has the correct version.
 */
void handle::_check_version() {
  // Find version symbol.
  logging::debug(logging::low)
    << "modules: checking module version (symbol " << versionning
    << ") in '" << _handle.fileName().toStdString() << "'";

  char const** version = (char const**)_handle.resolve(versionning);

  // Could not find version symbol.
  if (!version) {
    QString error_str(_handle.errorString());
    throw (exceptions::msg() << "modules: could not find "
                "version in '" << _handle.fileName()
             << "' (not a Centreon Broker module ?): " << error_str);
  }
  if (!*version)
    throw (exceptions::msg() << "modules: version symbol of module '"
           << _handle.fileName()
           << "' is empty (not a Centreon Broker module ?)");

  // Check version.
  if (::strcmp(CENTREON_BROKER_VERSION, *version) != 0)
    throw (exceptions::msg()
           << "modules: version mismatch in '" << _handle.fileName()
           << "': expected '" << CENTREON_BROKER_VERSION
           <<  "', found '" << *version << "'");
}

/**
 *  Call the module's initialization routine.
 *
 *  @param[in] arg Module argument.
 */
void handle::_init(void const* arg) {
  // Find initialization routine.
  union {
    void (* code)();
    void*   data;
  } sym;
  sym.data = _handle.resolve(initialization);

  // Could not find initialization routine.
  if (!sym.data) {
    QString error_str(_handle.errorString());
    throw (exceptions::msg() << "modules: could not find " \
                "initialization routine in '" << _handle.fileName()
             << "' (not a Centreon Broker module ?): " << error_str);
  }

  // Call initialization routine.
  logging::debug(logging::medium)
    << "modules: running initialization routine of '"
    << _handle.fileName().toStdString() << "'";
  (*(void (*)(void const*))(sym.code))(arg);

  return ;
}
