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

#include <QSqlDatabase>
#include "com/centreon/broker/bam/factory.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/bool_status.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exclusion.hh"

using namespace com::centreon::broker;

// Load count.
namespace {
  unsigned int instances(0);
  char const* bam_module("bam");
}

template <typename T>
void register_bam_event(io::events& e, bam::data_element de, const char* name) {
  e.register_event(
      io::events::bam,
      de,
      io::event_info(
            name,
            &T::operations,
            T::entries));
}

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    // Decrement instance number.
    if (!--instances) {
      // Deregister storage layer.
      io::protocols::instance().unreg(bam_module);
      // Deregister bam events.
      io::events::instance().unregister_category(io::events::bam);

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
      // BAM module.
      logging::info(logging::high)
        << "BAM: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // This is a workaround to keep a mysql driver open.
      if (!QSqlDatabase::contains())
        QSqlDatabase::addDatabase("QMYSQL");

      // Register storage layer.
      io::protocols::instance().reg(
                                  bam_module,
                                  bam::factory(),
                                  1,
                                  7);

      io::events& e(io::events::instance());

      // Register category.
      int bam_category(e.register_category("bam", io::events::bam));
      if (bam_category != io::events::bam) {
        e.unregister_category(bam_category);
        --instances;
        throw (exceptions::msg() << "bam: category " << io::events::bam
               << " is already registered whereas it should be "
               << "reserved for the bam module");
      }

      // Register bam events.
      {
        register_bam_event<bam::ba_status>(e, bam::de_ba_status, "ba_status");
        register_bam_event<bam::bool_status>(
                                  e,
                                  bam::de_bool_status,
                                  "bool_status");
        register_bam_event<bam::kpi_status>(
                                  e,
                                  bam::de_kpi_status,
                                  "kpi_status");
        register_bam_event<bam::meta_service_status>(
                                  e,
                                  bam::de_meta_service_status,
                                  "meta_service_status");
        register_bam_event<bam::ba_event>(e, bam::de_ba_event, "ba_event");
        register_bam_event<bam::kpi_event>(e, bam::de_kpi_event, "kpi_event");
        register_bam_event<bam::ba_duration_event>(
                                  e,
                                  bam::de_ba_duration_event,
                                  "ba_duration_event");
        register_bam_event<bam::dimension_ba_event>(
                                  e,
                                  bam::de_dimension_ba_event,
                                  "dimension_ba_event");
        register_bam_event<bam::dimension_kpi_event>(
                                  e,
                                  bam::de_dimension_kpi_event,
                                  "dimension_kpi_event");
        register_bam_event<bam::dimension_ba_bv_relation_event>(
                                  e,
                                  bam::de_dimension_ba_bv_relation_event,
                                  "dimension_ba_bv_relation_event");
        register_bam_event<bam::dimension_bv_event>(
                                  e,
                                  bam::de_dimension_bv_event,
                                  "dimension_bv_event");
        register_bam_event<bam::dimension_truncate_table_signal>(
                                  e,
                                  bam::de_dimension_truncate_table_signal,
                                  "dimension_truncate_table_signal");
        register_bam_event<bam::rebuild>(e, bam::de_rebuild, "rebuild");
        register_bam_event<bam::dimension_timeperiod>(
                                  e,
                                  bam::de_dimension_timeperiod,
                                  "dimension_timeperiod");
        register_bam_event<bam::dimension_ba_timeperiod_relation>(
                                  e,
                                  bam::de_dimension_ba_timeperiod_relation,
                                  "dimension_ba_timeperiod_relation");
        register_bam_event<bam::dimension_timeperiod_exception>(
                                  e,
                                  bam::de_dimension_timeperiod_exception,
                                  "dimension_ba_timeperiod_exception");
        register_bam_event<bam::dimension_timeperiod_exclusion>(
                                  e,
                                  bam::de_dimension_timeperiod_exclusion,
                                  "dimension_timeperiod_exclusion");
      }
    }
    return ;
  }
}
