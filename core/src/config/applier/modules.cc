/*
** Copyright 2011-2013 Merethis
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

#include <cstdlib>
#include <memory>
#include <QMutexLocker>
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker::config::applier;

// Class instance.
static modules* gl_modules = NULL;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
modules::~modules() {
  logging::debug(logging::high) << "module applier: destruction";
}

/**
 *  Apply a module configuration.
 *
 *  @param[in] module_list Module list.
 *  @param[in] module_dir  Module directory.
 *  @param[in] arg         Module argument.
 */
void modules::apply(
                QList<QString> const& module_list,
                QString const& module_dir,
                void const* arg) {
  // Lock multiplexing engine in case modules register hooks.
  QMutexLocker
    lock(&com::centreon::broker::multiplexing::engine::instance());

  // Load modules.
  for (QList<QString>::const_iterator
         it(module_list.begin()),
         end(module_list.end());
       it != end;
       ++it) {
    logging::config(logging::high)
      << "module applier: loading module '" << *it << "'";
    _loader.load_file(*it, arg);
  }
  if (!module_dir.isEmpty()) {
    logging::config(logging::high)
      << "module applier: loading directory '" << module_dir << "'";
    _loader.load_dir(module_dir, arg);
  }
  else
    logging::debug(logging::high)
      << "module applier: no directory defined";
  return ;
}

/**
 *  Get iterator to the first module.
 *
 *  @return Iterator to the first module.
 */
modules::iterator modules::begin() {
  return (_loader.begin());
}

/**
 *  Unload modules.
 */
void modules::discard() {
  _loader.unload();
  return ;
}

/**
 *  Get last iterator of the module list.
 *
 *  @return Last iterator of the module list.
 */
modules::iterator modules::end() {
  return (_loader.end());
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
modules& modules::instance() {
  return (*gl_modules);
}

/**
 *  Load the singleton.
 */
void modules::load() {
  if (!gl_modules)
    gl_modules = new modules;
  return ;
}

/**
 *  Unload the singleton.
 */
void modules::unload() {
  delete gl_modules;
  gl_modules = NULL;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
modules::modules() {}
