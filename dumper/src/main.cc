/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/dumper/factory.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"
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
    if (!--instances) {
      // Deregister storage layer.
      io::events::instance().unreg("dumper");
      io::protocols::instance().unreg("dumper");
    }
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration argument.
   */
  void broker_module_init(void const* arg) {
    (void)arg;

    // Increment instance number.
    if (!instances++) {
      // Dumper module.
      logging::info(logging::high)
        << "dumper: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Register dumper layer.
      io::protocols::instance().reg(
                                  "dumper",
                                  dumper::factory(),
                                  1,
                                  7);

      // Register dumper events.
      std::set<unsigned int> elements;
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_dump>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_reload>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_db_dump>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_entries_ba>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_entries_kpi>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_entries_host>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_entries_service>::value);
      io::events::instance().reg("dumper", elements);
    }
    return ;
  }
}
