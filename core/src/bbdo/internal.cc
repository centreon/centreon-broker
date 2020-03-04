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

#include "com/centreon/broker/bbdo/internal.hh"

#include "com/centreon/broker/bbdo/ack.hh"
#include "com/centreon/broker/bbdo/factory.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
 *                                     *
 *           Global Objects            *
 *                                     *
 **************************************/

/**
 *  @brief BBDO initialization routine.
 *
 *  Initialize BBDO mappings and register BBDO protocol.
 */
void bbdo::load() {
  // Register BBDO category.
  io::events& e(io::events::instance());
  int bbdo_category(e.register_category("bbdo", io::events::bbdo));
  if (bbdo_category != io::events::bbdo) {
    e.unregister_category(bbdo_category);
    log_v2::instance().bbdo()->error(
        "BBDO: category {} is already registered whereas it should be reserved "
        "for the BBDO core",
        io::events::bbdo);
    throw(exceptions::msg() << "BBDO: category " << io::events::bbdo
                            << " is already registered whereas it should be "
                            << "reserved for the BBDO core");
  }

  // Register BBDO events.
  e.register_event(
      io::events::bbdo, bbdo::de_version_response,
      io::event_info("version_response", &version_response::operations,
                     version_response::entries));
  e.register_event(io::events::bbdo, bbdo::de_ack,
                   io::event_info("ack", &ack::operations, ack::entries));

  // Register BBDO protocol.
  io::protocols::instance().reg("BBDO", std::make_shared<bbdo::factory>(), 7,
                                7);
}

/**
 *  @brief BBDO cleanup routine.
 *
 *  Delete BBDO mappings and unregister BBDO protocol.
 */
void bbdo::unload() {
  // Unregister protocol.
  io::protocols::instance().unreg("BBDO");

  // Unregister category.
  io::events::instance().unregister_category(io::events::bbdo);
}
