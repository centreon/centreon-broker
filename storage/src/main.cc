/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/factory.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/stream.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/status.hh"

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
    if (!--instances) {
      // Deregister storage layer.
      // Remove events.
      io::events::instance().unregister_category(io::events::storage);
      io::protocols::instance().unreg("storage");
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

      io::events& e(io::events::instance());

      // Register category.
      int storage_category(e.register_category("storage", io::events::storage));
      if (storage_category != io::events::storage) {
        e.unregister_category(storage_category);
        --instances;
        throw (exceptions::msg() << "storage: category " << io::events::storage
               << " is already registered whereas it should be "
               << "reserved for the storage module");
      }

      // Register events.
      {
        e.register_event(
            io::events::storage,
            storage::de_metric,
            io::event_info(
                  "metric",
                  &storage::metric::operations,
                  storage::metric::entries,
                  "rt_metrics"));
        e.register_event(
            io::events::storage,
            storage::de_rebuild,
            io::event_info(
                  "rebuild",
                  &storage::rebuild::operations,
                  storage::rebuild::entries));
        e.register_event(
            io::events::storage,
            storage::de_remove_graph,
            io::event_info(
                  "remove_graph",
                  &storage::remove_graph::operations,
                  storage::remove_graph::entries));
        e.register_event(
            io::events::storage,
            storage::de_status,
            io::event_info(
                  "status",
                  &storage::status::operations,
                  storage::status::entries));
        e.register_event(
            io::events::storage,
            storage::de_index_mapping,
            io::event_info(
                  "index_mapping",
                  &storage::index_mapping::operations,
                  storage::index_mapping::entries));
        e.register_event(
            io::events::storage,
            storage::de_metric_mapping,
            io::event_info(
                  "metric_mapping",
                  &storage::metric_mapping::operations,
                  storage::metric_mapping::entries));
      }

      // Register storage layer.
      io::protocols::instance().reg(
                                  "storage",
                                  storage::factory(),
                                  1,
                                  7);
    }
    return ;
  }
}
