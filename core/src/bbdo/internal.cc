/*
** Copyright 2013,2015 Merethis
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

#include "com/centreon/broker/bbdo/factory.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

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
    throw (exceptions::msg() << "BBDO: category " << io::events::bbdo
           << " is already registered whereas it should be "
           << "reserved for the BBDO core");
  }

  // Register BBDO events.
  e.register_event(
      io::events::bbdo,
      bbdo::de_version_response,
      io::event_info(
            "version_response",
            &version_response::operations,
            version_response::entries));

  // Register BBDO protocol.
  io::protocols::instance().reg(
                              "BBDO",
                              bbdo::factory(),
                              7,
                              7);

  return ;
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

  return ;
}
