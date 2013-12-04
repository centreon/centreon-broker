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
#include "com/centreon/broker/storage/factory.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/stream.hh"

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
      io::events::instance().unreg("storage");
      io::protocols::instance().unreg("storage");

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
        << "storage: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // This is a workaround to keep a mysql driver open.
      if (!QSqlDatabase::contains())
        QSqlDatabase::addDatabase("QMYSQL");

      // Register storage layer.
      io::protocols::instance().reg(
                                  "storage",
                                  storage::factory(),
                                  1,
                                  7);

      // Register storage events.
      std::set<unsigned int> elements;
      elements.insert(
                 io::events::data_type<io::events::storage, storage::de_metric>::value);
      elements.insert(
                 io::events::data_type<io::events::storage, storage::de_rebuild>::value);
      elements.insert(
                 io::events::data_type<io::events::storage, storage::de_remove_graph>::value);
      elements.insert(
                 io::events::data_type<io::events::storage, storage::de_status>::value);
      io::events::instance().reg("storage", elements);
    }
    return ;
  }
}
