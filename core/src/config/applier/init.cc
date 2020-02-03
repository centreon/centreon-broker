/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/config/applier/init.hh"
#include <cstdlib>
#include <memory>
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/compression/internal.hh"
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
//#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/file/internal.hh"
#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/time/timezone_manager.hh"

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
  config::applier::endpoint::unload();
  config::applier::logger::unload();
  config::applier::state::unload();
  bbdo::unload();
  compression::unload();
  //extcmd::unload();
  file::unload();
  multiplexing::engine::instance().clear();
  config::applier::modules::unload();
  multiplexing::engine::unload();
  io::protocols::unload();
  io::events::unload();
}

/**
 *  Load necessary structures.
 */
void config::applier::init() {
  // Load singletons.
  multiplexing::engine::load();
  io::events::load();
  io::protocols::load();
  config::applier::modules::load();
  file::load();
  //extcmd::load();
  instance_broadcast::load();
  compression::load();
  bbdo::load();
  config::applier::logger::load();
  config::applier::endpoint::load();
  config::applier::state::load();
}
