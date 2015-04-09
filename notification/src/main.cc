/*
** Copyright 2011-2015 Merethis
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
#include "com/centreon/broker/notification/acknowledgement.hh"
#include "com/centreon/broker/notification/acknowledgement_removed.hh"
#include "com/centreon/broker/notification/downtime.hh"
#include "com/centreon/broker/notification/downtime_removed.hh"
#include "com/centreon/broker/notification/factory.hh"
#include "com/centreon/broker/notification/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"

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
      io::protocols::instance().reg(
        "notification",
        notification::factory(),
        1,
        7);

      // Register category.
      io::events& e(io::events::instance());
      int ret(e.register_category(
                  "notification",
                  io::events::notification));
      if (ret != io::events::notification) {
        e.unregister_category(ret);
        --instances;
        throw (exceptions::msg() << "notification: category "
               << io::events::notification
               << " is already registered whereas it should be "
               << "reserved for the notification module");
      }

      // Register event.
      e.register_event(
        io::events::notification,
        notification::de_acknowledgement,
        io::event_info(
              "acknowledgement",
              &notification::acknowledgement::operations,
              notification::acknowledgement::entries));
      e.register_event(
        io::events::notification,
        notification::de_acknowledgement_removed,
        io::event_info(
              "acknowledgement_removed",
              &notification::acknowledgement_removed::operations,
              notification::acknowledgement_removed::entries));
      e.register_event(
          io::events::notification,
          notification::de_acknowledgement,
          io::event_info(
                "downtime",
                &notification::downtime::operations,
                notification::downtime::entries));
      e.register_event(
        io::events::notification,
        notification::de_acknowledgement_removed,
        io::event_info(
              "downtime_removed",
              &notification::downtime_removed::operations,
              notification::downtime_removed::entries));
    }

    return ;
  }
}
