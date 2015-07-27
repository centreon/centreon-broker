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

#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/organization.hh"
#include "com/centreon/broker/dumper/factory.hh"
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/reload.hh"
#include "com/centreon/broker/dumper/remove.hh"
#include "com/centreon/broker/dumper/timestamp_cache.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

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
      io::events::instance().unregister_category(io::events::dumper);
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


      io::events& e(io::events::instance());

      // Register category.
      int dumper_category(e.register_category("dumper", io::events::dumper));
      if (dumper_category != io::events::dumper) {
        e.unregister_category(dumper_category);
        --instances;
        throw (exceptions::msg() << "dumper: category " << io::events::dumper
               << " is already registered whereas it should be "
               << "reserved for the dumper module");
      }

      // Register events.
      {
        e.register_event(
            io::events::dumper,
            dumper::de_dump,
            io::event_info(
                  "dump",
                  &dumper::dump::operations,
                  dumper::dump::entries));
        e.register_event(
            io::events::dumper,
            dumper::de_timestamp_cache,
            io::event_info(
                  "timestamp_cache",
                  &dumper::timestamp_cache::operations,
                  dumper::dump::entries));
        e.register_event(
            io::events::dumper,
            dumper::de_remove,
            io::event_info(
                  "remove",
                  &dumper::remove::operations,
                  dumper::remove::entries));
        e.register_event(
            io::events::dumper,
            dumper::de_reload,
            io::event_info(
                  "reload",
                  &dumper::reload::operations,
                  dumper::reload::entries));
        e.register_event(
            io::events::dumper,
            dumper::de_db_dump,
            io::event_info(
                  "db_dump",
                  &dumper::db_dump::operations,
                  dumper::db_dump::entries));
        e.register_event(
            io::events::dumper,
            dumper::de_entries_ba,
            io::event_info(
                  "ba",
                  &dumper::entries::ba::operations,
                  dumper::entries::ba::entries,
                  "cfg_bam"));
        e.register_event(
            io::events::dumper,
            dumper::de_entries_ba_type,
            io::event_info(
                  "ba_type",
                  &dumper::entries::ba_type::operations,
                  dumper::entries::ba_type::entries,
                  "cfg_bam_ba_types"));
        e.register_event(
            io::events::dumper,
            dumper::de_entries_kpi,
            io::event_info(
                  "kpi",
                  &dumper::entries::kpi::operations,
                  dumper::entries::kpi::entries,
                  "cfg_bam_kpi"));
        e.register_event(
            io::events::dumper,
            dumper::de_entries_organization,
            io::event_info(
                  "organization",
                  &dumper::entries::organization::operations,
                  dumper::entries::organization::entries,
                  "cfg_organizations"));
      }


      // Register dumper layer.
      io::protocols::instance().reg(
                                  "dumper",
                                  dumper::factory(),
                                  1,
                                  7);
    }
    return ;
  }
}
