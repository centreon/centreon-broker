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
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_entries_boolean>::value);
      elements.insert(
                 io::events::data_type<io::events::dumper, dumper::de_db_dump_committed>::value);
      io::events::instance().reg("dumper", elements);
    }
    return ;
  }
}
