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

#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/local/factory.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

extern "C" {
  /**
   *  Module deinitialization routine.
   *
   *  @param[in] force true if module unloading is forced (no need to
   *                   cleanup).
   */
  void broker_module_deinit(bool force) {
    // Decrement instance number.
    if (!--instances && !force)
      // Unregister local sockets.
      io::protocols::instance().unreg("local");
    return ;
  }

  /**
   *  Module initialization routine.
   */
  void broker_module_init() {
    // Increment instance number.
    if (!instances++)
      // Register local sockets.
      io::protocols::instance().reg("local",
        local::factory(),
        1,
        3);
    return ;
  }
}
