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

#include <rrd.h>
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/factory.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

extern "C" {
  /**
   *  Module version symbol. Used to check for version mismatch.
   */
  char const* broker_module_version = CENTREON_BROKER_VERSION;

  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances)
      // Deregister RRD layer.
      io::protocols::instance().unreg("RRD");
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration object.
   */
  void broker_module_init(void const* arg) {
    (void)arg;

    // Increment instance number.
    if (!instances++) {
      // RRD module.
      logging::info(logging::high)
        << "RRD: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Print RRDtool version.
      char const* rrdversion(rrd_strversion());
      logging::info(logging::high) << "RRD: using rrdtool "
        << (rrdversion ? rrdversion : "(unknown)");

      // Register RRD layer.
      io::protocols::instance().reg("RRD",
        rrd::factory(),
        1,
        7);
    }

    return ;
  }
}
