/*
** Copyright 2011-2012 Merethis
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
#include <QAbstractSocket>
#include <QtCore>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/applier/temporary.hh"
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
  config::applier::modules::unload();
  multiplexing::engine::unload();
  io::protocols::unload();
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
  io::protocols::load();
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
