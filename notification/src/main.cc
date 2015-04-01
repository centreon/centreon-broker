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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/acknowledgement_removed.hh"
#include "com/centreon/broker/notification/downtime_removed.hh"
#include "com/centreon/broker/notification/factory.hh"
#include "com/centreon/broker/notification/internal.hh"

using namespace com::centreon::broker;

// Load count.
static unsigned int instances(0);

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances) {
      // Deregister notification layer.
      io::protocols::instance().unreg("notification");

      // Deregister events.
      io::events::instance().unregister_category(io::events::notification);
    }
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
      // Notification module.
      logging::info(logging::high)
        << "notification: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Register Notification layer.
      io::protocols::instance().reg("notification",
        notification::factory(),
        1,
        7);

      // Register events.
      io::events& e(io::events::instance());
      // Register event.
      e.register_event(
        io::events::notification,
        notification::de_acknowledgement_removed,
          io::event_info(
                "command",
                &notification::acknowledgement_removed::operations,
                notification::acknowledgement_removed::entries));
      e.register_event(
        io::events::notification,
        notification::de_acknowledgement_removed,
          io::event_info(
                "command",
                &notification::acknowledgement_removed::operations,
                notification::acknowledgement_removed::entries));
    }

    return ;
  }
}
