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

#include <assert.h>
#include <stdlib.h>
#include "config/applier/modules.hh"
#include "logging/logging.hh"

using namespace com::centreon::broker::config::applier;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
modules::modules() {}

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] m Object to copy.
 */
modules::modules(modules const& m) {
  (void)m;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] m Object to copy.
 *
 *  @return This object.
 */
modules& modules::operator=(modules const& m) {
  (void)m;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
modules::~modules() {}

/**
 *  Apply a module configuration.
 *
 *  @param[in] module_dir Module directory.
 */
void modules::apply(QString const& module_dir) {
  if (!module_dir.isEmpty()) {
    logging::config << logging::HIGH << "module applier: loading directory";
    _loader.load_dir(module_dir);
  }
  else
    logging::debug << logging::HIGH << "module applier: no directory defined";
  return ;
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
modules& modules::instance() {
  static modules gl_modules;
  return (gl_modules);
}
