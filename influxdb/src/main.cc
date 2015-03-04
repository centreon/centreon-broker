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

#include <QSqlDatabase>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/influxdb/factory.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/influxdb/stream.hh"

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
      // Deregister storage layer.
      io::events::instance().unreg("influxdb");
      io::protocols::instance().unreg("influxdb");

      // Remove the workaround connection.
      if (QSqlDatabase::contains())
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
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
      // Storage module.
      logging::info(logging::high)
        << "influxdb: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Register storage layer.
      io::protocols::instance().reg(
                                  "influxdb",
                                  influxdb::factory(),
                                  1,
                                  7);
    }
    return ;
  }
}
