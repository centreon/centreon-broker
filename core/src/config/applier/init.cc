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

#include <cstdlib>
#include <memory>
#include <QAbstractSocket>
#include <QtCore>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/applier/temporary.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/temporary.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;

// Declare type.
Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

/**************************************
*                                     *
*           Global Functions          *
*                                     *
**************************************/

/**
 *  Unload necessary structures.
 */
void config::applier::deinit() {
  config::applier::state::unload();
  config::applier::endpoint::unload();
  config::applier::temporary::unload();
  config::applier::logger::unload();
  io::temporary::unload();
  multiplexing::engine::instance().clear();
  config::applier::modules::unload();
  extcmd::unload();
  multiplexing::engine::unload();
  io::protocols::unload();
  io::events::unload();
  logging::manager::unload();
  return ;
}

/**
 *  Load necessary structures.
 */
void config::applier::init() {
  // Load singletons.
  logging::manager::load();
  io::temporary::load();
  multiplexing::engine::load();
  io::events::load();
  io::protocols::load();
  extcmd::load();
  config::applier::modules::load();
  config::applier::logger::load();
  config::applier::temporary::load();
  config::applier::endpoint::load();
  config::applier::state::load();

  // Register Qt types.
  qRegisterMetaType<QAbstractSocket::SocketError>(
    "QAbstractSocket::SocketError");
  qRegisterMetaType<QAbstractSocket::SocketState>(
    "QAbstractSocket::SocketState");

  return ;
}
