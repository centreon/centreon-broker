/*
** Copyright 2011 Merethis
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

#include <stdlib.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Global Functions          *
*                                     *
**************************************/

/**
 *  Unload necessary structures.
 */
void config::applier::deinit() {
  multiplexing::engine::unload();
  logging::manager::unload();
  return ;
}

/**
 *  Load necessary structures.
 */
void config::applier::init() {
  logging::manager::load();
  multiplexing::engine::load();
  atexit(&deinit);
  return ;
}
