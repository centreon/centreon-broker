/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/bbdo/factory.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances)
      // Deregister BBDO protocol.
      io::protocols::instance().unreg("BBDO");
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
      // BBDO module.
      logging::info(logging::high)
        << "BBDO: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Print protocol version.
      logging::info(logging::high)
        << "BBDO: using protocol version " << BBDO_VERSION_MAJOR
        << "." << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;
      // Register BBDO protocol.
      io::protocols::instance().reg(
                                  "BBDO",
                                  bbdo::factory(),
                                  7,
                                  7);
      // Initialize mappings.
      bbdo::initialize();
    }

    return ;
  }
}
