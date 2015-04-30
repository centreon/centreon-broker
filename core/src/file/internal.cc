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

#include "com/centreon/broker/file/factory.hh"
#include "com/centreon/broker/file/internal.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

void file::unload() {
  // Unregister file layer.
  io::protocols::instance().unreg("file");
}

void file::load() {
  // File module.
  logging::info(logging::high)
    << "file: module for Centreon Broker "
    << CENTREON_BROKER_VERSION;

  // Register file layer.
  io::protocols::instance().reg("file",
    file::factory(),
    1,
    3);
}
