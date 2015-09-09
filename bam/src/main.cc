/*
** Copyright 2011-2014 Centreon
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

#include <QSqlDatabase>
#include "com/centreon/broker/bam/factory.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/bam/time/timezone_manager.hh"

using namespace com::centreon::broker;

// Load count.
namespace {
  unsigned int instances(0);
  char const* bam_module("bam");
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
      io::events::instance().unreg(bam_module);

      // Remove the workaround connection.
      if (QSqlDatabase::contains())
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

      // Deinitialize timezone manager.
      bam::time::timezone_manager::unload();
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

      // Load timezone manager.
      bam::time::timezone_manager::load();

      // Register bam events.
      std::set<unsigned int> elements;
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_ba_status>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_bool_status>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_kpi_status>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_meta_service_status>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_ba_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_kpi_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_ba_duration_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_ba_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_kpi_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_ba_bv_relation_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_bv_event>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_truncate_table_signal>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_rebuild>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_timeperiod>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_ba_timeperiod_relation>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_timeperiod_exception>::value);
      elements.insert(
                 io::events::data_type<io::events::bam, bam::de_dimension_timeperiod_exclusion>::value);
      io::events::instance().reg(bam_module, elements);
    }
    return ;
  }
}
