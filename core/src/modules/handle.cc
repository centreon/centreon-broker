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
char const* handle::deinitialization = "broker_module_deinit";
char const* handle::initialization = "broker_module_init";

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Call the module's initialization routine.
 *
 *  @param[in] arg Module argument.
 */
void handle::_init(void const* arg) {
  // Find initialization routine.
  logging::debug << logging::LOW << "modules: searching "
       "initialization routine (symbol " << initialization
    << ") in '" << _handle.fileName() << "'";
  void* sym(_handle.resolve(initialization));

  // Could not find initialization routine.
  if (!sym) {
    QString error_str(_handle.errorString());
    throw (exceptions::msg() << "modules: could not find " \
                "initialization routine in '" << _handle.fileName()
             << "' (not a Centreon Broker module ?): " << error_str);
  }

  // Call initialization routine.
  logging::debug << logging::LOW << "modules: calling initialization " \
    "routine of '" << _handle.fileName() << "'";
  (*(void (*)(void const*))(sym))(arg);
  logging::debug << logging::MEDIUM << "modules: initialization " \
    "routine of '" << _handle.fileName() << "' successfully completed";

  return ;
}

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
 *  @param[in] h Object to copy.
 */
handle::handle(handle const& h) {
  this->open(h._handle.fileName());
}

/**
 *  Destructor.
 */
handle::~handle() {
  try {
    this->close();
  }
  catch (std::exception const& e) {
    logging::error << logging::HIGH << e.what();
  }
  catch (...) {
    logging::error << logging::HIGH << "modules: unknown error while " \
      "unloading '" << _handle.fileName() << "'";
  }
}

/**
 *  Assignment operator.
 *
 *  @param[in] h Object to copy.
 *
 *  @return This object.
 */
handle& handle::operator=(handle const& h) {
  this->close();
  this->open(h._handle.fileName());
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
    logging::info << logging::MEDIUM << "modules: closing '"
      << _handle.fileName() << "'";

    // Find deinitialization routine.
    logging::debug << logging::LOW << "modules: searching " \
         "deinitialization routine (symbol " << deinitialization
      << ") in '" << _handle.fileName() << "'";
    void* sym(_handle.resolve(deinitialization));

    // Could not find deinitialization routine.
    if (!sym) {
      QString error_str(_handle.errorString());
      logging::info << logging::MEDIUM << "modules: could not find " \
           "deinitialization routine in '" << _handle.fileName()
        << "': " << error_str;
    }
    // Call deinitialization routine.
    else {
      logging::debug << logging::LOW << "modules: calling " \
        "deinitialization routine of '" << _handle.fileName() << "'";
      (*(void (*)(bool))(sym))(true);
      logging::debug << logging::LOW << "modules: deinitialization " \
           "routine of '" << _handle.fileName()
        << "' successfully completed";
    }

    // Reset library handle.
    logging::debug << logging::LOW << "modules: unloading library '"
      << _handle.fileName() << "'";
    // Library was not unloaded.
    if (!_handle.unload()) {
      QString error_str(_handle.errorString());
      logging::info << logging::MEDIUM
        << "modules: could not unload library '"
        << _handle.fileName() << "': " << error_str;
    }
    // Library was unloaded.
    else
      logging::info << logging::MEDIUM << "modules: library '"
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
void handle::open(QString const& filename, void const* arg) {
  // Close library if previously open.
  this->close();

  // Load library.
  logging::debug << logging::MEDIUM << "modules: loading library '"
    << filename << "'";
  _handle.setFileName(filename);
  _handle.setLoadHints(QLibrary::ResolveAllSymbolsHint
    | QLibrary::ExportExternalSymbolsHint);

  // Could not load library.
  if (!_handle.load())
    throw (exceptions::msg() << "modules: could not load library '"
             << filename << "': " << _handle.errorString());
  logging::info << logging::MEDIUM << "modules: library '"
    << filename << "' loaded";

  // Initialize module.
  _init(arg);

  return ;
}
