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

#include "exceptions/basic.hh"
#include "modules/handle.hh"

using namespace com::centreon::broker::modules;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Call the module's initialization routine.
 */
void handle::_init() {
  // Initialize module.
  void* sym(_handle->resolve("broker_module_init"));
  if (!sym)
    throw (exceptions::basic() << "could not find module initialization routine in '"
             << _handle->fileName().toStdString().c_str()
             << "': " << _handle->errorString().toStdString().c_str());
  (*(void (*)())(sym))();
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
handle::handle() : _handle(new QLibrary) {}

/**
 *  Copy constructor.
 *
 *  @param[in] h Object to copy.
 */
handle::handle(handle const& h) : _handle(h._handle) {
  _init();
}

/**
 *  Destructor.
 */
handle::~handle() {
  this->close();
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
  _handle = h._handle;
  _init();
  return (*this);
}

/**
 *  Close the library object.
 */
void handle::close() {
  if (is_open()) {
    // Call deinit function.
    void* sym(_handle->resolve("broker_module_deinit"));
    if (!sym)
      throw (exceptions::basic() << "could not find module deinitialiation routine in '"
               << _handle->fileName().toStdString().c_str()
               << "': " << _handle->errorString().toStdString().c_str());
    (*(void (*)())(sym))();

    // Reset library handle.
    _handle.clear();
  }
  return ;
}

/**
 *  Check if the library is loaded.
 *
 *  @return true if the library is loaded, false otherwise.
 */
bool handle::is_open() const {
  return (!_handle.isNull() && _handle->isLoaded());
}

/**
 *  Load a library file.
 *
 *  @param[in] filename Library filename.
 */
void handle::open(QString const& filename) {
  // Close library if previously open.
  this->close();

  // Load library.
  _handle->setFileName(filename);
  if (!_handle->load())
    throw (exceptions::basic() << "could not load module '"
             << filename.toStdString().c_str()
             << "': " << _handle->errorString().toStdString().c_str());

  // Call module's initialization routine.
  _init();

  return ;
}
