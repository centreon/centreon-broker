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

#include <cstdlib>
#include <memory>
#include <QAbstractSocket>
#include <QtCore>
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/compression/internal.hh"
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/file/internal.hh"
#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/time/timezone_manager.hh"

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
  config::applier::endpoint::unload();
  config::applier::logger::unload();
  config::applier::state::unload();
  bbdo::unload();
  compression::unload();
  extcmd::unload();
  file::unload();
  multiplexing::engine::instance().clear();
  config::applier::modules::unload();
  multiplexing::engine::unload();
  io::protocols::unload();
  io::events::unload();
  time::timezone_manager::unload();
  logging::manager::unload();
  return ;
}

/**
 *  Load necessary structures.
 */
void config::applier::init() {
  // Load singletons.
  logging::manager::load();
  time::timezone_manager::load();
  multiplexing::engine::load();
  io::events::load();
  io::protocols::load();
  config::applier::modules::load();
  file::load();
  extcmd::load();
  instance_broadcast::load();
  compression::load();
  bbdo::load();
  config::applier::logger::load();
  config::applier::endpoint::load();
  config::applier::state::load();

  // Register Qt types.
  qRegisterMetaType<QAbstractSocket::SocketError>(
    "QAbstractSocket::SocketError");
  qRegisterMetaType<QAbstractSocket::SocketState>(
    "QAbstractSocket::SocketState");

  return ;
}
