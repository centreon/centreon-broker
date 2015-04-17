/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/compression/factory.hh"
#include "com/centreon/broker/compression/internal.hh"
#include "com/centreon/broker/io/protocols.hh"

using namespace com::centreon::broker;

/**
 *  Register the compression protocol.
 */
void compression::load() {
  // Register compression layer.
  io::protocols::instance().reg(
                              "compression",
                              compression::factory(),
                              6,
                              6);
  return ;
}

/**
 *  Unregister the compression protocol.
 */
void compression::unload() {
  // Unregister compression layer.
  io::protocols::instance().unreg("compression");
  return ;
}
