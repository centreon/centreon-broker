/*
** Copyright 2011-2013,2015 Merethis
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
      "unloading '" << _handle.fileName() << "'";
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
      << _handle.fileName() << "'";

    // Find deinitialization routine.
    logging::debug(logging::low) << "modules: searching " \
         "deinitialization routine (symbol " << deinitialization
      << ") in '" << _handle.fileName() << "'";
    union {
      void (* code)();
      void*   data;
    } sym;
    sym.data = _handle.resolve(deinitialization);

    // Could not find deinitialization routine.
    if (!sym.data) {
      QString error_str(_handle.errorString());
      logging::info(logging::medium) << "modules: could not find " \
           "deinitialization routine in '" << _handle.fileName()
        << "': " << error_str;
    }
    // Call deinitialization routine.
    else {
      (*(sym.code))();
      logging::debug(logging::low) << "modules: deinitialization " \
           "routine of '" << _handle.fileName()
        << "' successfully completed";
    }

    // Reset library handle.
    logging::debug(logging::low) << "modules: unloading library '"
      << _handle.fileName() << "'";
    // Library was not unloaded.
    if (!_handle.unload()) {
      QString error_str(_handle.errorString());
      logging::info(logging::medium)
        << "modules: could not unload library '"
        << _handle.fileName() << "': " << error_str;
    }
    // Library was unloaded.
    else
      logging::info(logging::medium) << "modules: library '"
        << _handle.fileName() << "' unloaded";
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
  logging::info(logging::medium) << "modules: library '"
    << filename << "' loaded";

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
  logging::debug(logging::low) << "modules: searching update "
    << "routine (symbol " << updatization << ") in '"
    << _handle.fileName() << "'";
  union {
    void (* code)();
    void*   data;
  } sym;
  sym.data = _handle.resolve(updatization);

  // Found routine.
  if (sym.data) {
    (*(void (*)(void const*))(sym.code))(arg);
    logging::debug(logging::low) << "modules: update routine of '"
      << _handle.fileName() << "' successfully completed";
  }
  // Routine not found.
  else
    logging::info(logging::medium) << "modules: could not find update "
      << "routine of module '" << _handle.fileName() << "'";

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
  logging::debug(logging::low) << "modules: searching "
       "version (symbol " << versionning
    << ") in '" << _handle.fileName() << "'";

  char const** version = (char const**)_handle.resolve(versionning);

  // Could not find version symbol.
  if (!version) {
    QString error_str(_handle.errorString());
    throw (exceptions::msg() << "modules: could not find " \
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
           <<  "', found '" << version << "'");
}

/**
 *  Call the module's initialization routine.
 *
 *  @param[in] arg Module argument.
 */
void handle::_init(void const* arg) {
  // Find initialization routine.
  logging::debug(logging::low) << "modules: searching "
       "initialization routine (symbol " << initialization
    << ") in '" << _handle.fileName() << "'";
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
  (*(void (*)(void const*))(sym.code))(arg);
  logging::debug(logging::medium) << "modules: initialization " \
    "routine of '" << _handle.fileName() << "' successfully completed";

  return ;
}
