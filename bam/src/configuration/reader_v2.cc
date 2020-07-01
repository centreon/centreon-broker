/*
** Copyright 2014-2017 Centreon
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

#include "com/centreon/broker/bam/configuration/reader_v2.hh"

#include <fmt/format.h>

#include <cstring>
#include <memory>

#include "com/centreon/broker/bam/configuration/reader_exception.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/time/timeperiod.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] centreon_db  Centreon database connection.
 *  @param[in] storage_cfg  Storage database configuration.
 */
reader_v2::reader_v2(mysql& centreon_db, database_config const& storage_cfg)
    : _mysql(centreon_db), _storage_cfg(storage_cfg) {}

/**
 *  Destructor.
 */
reader_v2::~reader_v2() {}

/**
 *  Read configuration from database.
 *
 *  @param[out] st  All the configuration state for the BA subsystem
 *                  recuperated from the specified database.
 */
void reader_v2::read(state& st) {
  try {
    _load_dimensions();
    _load(st.get_bas(), st.get_ba_svc_mapping());
    _load(st.get_kpis());
    _load(st.get_bool_exps());
    _load(st.get_meta_services());
    _load(st.get_hst_svc_mapping());
  } catch (std::exception const& e) {
    st.clear();
    throw;
  }
}

/**
 *  Load KPIs from the DB.
 *
 *  @param[out] kpis The list of kpis in database.
 */
void reader_v2::_load(state::kpis& kpis) {
  try {
    std::string query(fmt::format(
        "SELECT  k.kpi_id, k.state_type, k.host_id, k.service_id, k.id_ba,"
        "        k.id_indicator_ba, k.meta_id, k.boolean_id,"
        "        k.current_status, k.last_level, k.downtime,"
        "        k.acknowledged, k.ignore_downtime,"
        "        k.ignore_acknowledged,"
        "        COALESCE(COALESCE(k.drop_warning, ww.impact), "
        "g.average_impact),"
        "        COALESCE(COALESCE(k.drop_critical, cc.impact), "
        "g.average_impact),"
        "        COALESCE(COALESCE(k.drop_unknown, uu.impact), "
        "g.average_impact),"
        "        k.last_state_change, k.in_downtime, k.last_impact"
        "  FROM mod_bam_kpi AS k"
        "  INNER JOIN mod_bam AS mb"
        "    ON k.id_ba = mb.ba_id"
        "  INNER JOIN mod_bam_poller_relations AS pr"
        "    ON pr.ba_id = mb.ba_id"
        "  LEFT JOIN mod_bam_impacts AS ww"
        "    ON k.drop_warning_impact_id = ww.id_impact"
        "  LEFT JOIN mod_bam_impacts AS cc"
        "    ON k.drop_critical_impact_id = cc.id_impact"
        "  LEFT JOIN mod_bam_impacts AS uu"
        "    ON k.drop_unknown_impact_id = uu.id_impact"
        "  LEFT JOIN (SELECT id_ba, 100.0 / COUNT(kpi_id) AS average_impact"
        "               FROM mod_bam_kpi"
        "               WHERE activate='1'"
        "               GROUP BY id_ba) AS g"
        "    ON k.id_ba=g.id_ba"
        "  WHERE k.activate='1'"
        "    AND mb.activate='1'"
        "    AND pr.poller_id={}",
        config::applier::state::instance().poller_id()));
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        // KPI object.
        uint32_t kpi_id(res.value_as_u32(0));
        kpis[kpi_id] = kpi(kpi_id,                 // ID.
                           res.value_as_i32(1),    // State type.
                           res.value_as_u32(2),    // Host ID.
                           res.value_as_u32(3),    // Service ID.
                           res.value_as_u32(4),    // BA ID.
                           res.value_as_u32(5),    // BA indicator ID.
                           res.value_as_u32(6),    // Meta-service ID.
                           res.value_as_u32(7),    // Boolean expression ID.
                           res.value_as_i32(8),    // Status.
                           res.value_as_i32(9),    // Last level.
                           res.value_as_f32(10),   // Downtimed.
                           res.value_as_f32(11),   // Acknowledged.
                           res.value_as_bool(12),  // Ignore downtime.
                           res.value_as_bool(13),  // Ignore acknowledgement.
                           res.value_as_f64(14),   // Warning.
                           res.value_as_f64(15),   // Critical.
                           res.value_as_f64(16));  // Unknown.

        // KPI state.
        if (!res.value_is_null(17)) {
          kpi_event e;
          e.kpi_id = kpi_id;
          e.ba_id = res.value_as_u32(4);
          e.status = res.value_as_i32(8);
          e.start_time = res.value_as_u64(17);
          e.in_downtime = res.value_as_bool(18);
          e.impact_level = res.value_is_null(19) ? -1 : res.value_as_f64(19);
          kpis[kpi_id].set_opened_event(e);
        }
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "BAM: could not retrieve KPI configuration from DB: " << e.what();
    }

    // Load host ID/service ID of meta-services (temporary fix until
    // Centreon Broker 3 where meta-services will be computed by Broker
    // itself.
    for (state::kpis::iterator it(kpis.begin()), end(kpis.end()); it != end;
         ++it) {
      if (it->second.is_meta()) {
        std::string query(
            fmt::format("SELECT hsr.host_host_id, hsr.service_service_id"
                        "  FROM service AS s"
                        "  LEFT JOIN host_service_relation AS hsr"
                        "    ON s.service_id=hsr.service_service_id"
                        "  WHERE s.service_description='meta_{}'",
                        it->second.get_meta_id()));
        std::promise<database::mysql_result> promise;
        _mysql.run_query_and_get_result(query, &promise);
        try {
          database::mysql_result res(promise.get_future().get());
          if (!_mysql.fetch_row(res))
            throw exceptions::msg() << "virtual service of meta-service "
                                    << it->first << " does not exist";
          it->second.set_host_id(res.value_as_u32(0));
          it->second.set_service_id(res.value_as_u32(1));
        } catch (std::exception const& e) {
          throw exceptions::msg()
              << "could not retrieve virtual meta-service's service: "
              << e.what();
        }
      }
    }
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve KPI configuration from DB: " << e.what();
  }
}

/**
 *  Load BAs from the DB.
 *
 *  @param[out] bas      The list of BAs in database.
 *  @param[out] mapping  The mapping of BA ID to host name/service
 *                       description.
 */
void reader_v2::_load(state::bas& bas, bam::ba_svc_mapping& mapping) {
  try {
    {
      std::string query(
          fmt::format("SELECT b.ba_id, b.name, b.state_source, b.level_w,"
                      "       b.level_c, b.last_state_change, b.current_status,"
                      "       b.in_downtime, b.inherit_kpi_downtimes"
                      "  FROM mod_bam AS b"
                      "  INNER JOIN mod_bam_poller_relations AS pr"
                      "    ON b.ba_id=pr.ba_id"
                      "  WHERE b.activate='1'"
                      "    AND pr.poller_id={}",
                      config::applier::state::instance().poller_id()));
      std::promise<database::mysql_result> promise;
      _mysql.run_query_and_get_result(query, &promise);
      try {
        database::mysql_result res(promise.get_future().get());
        while (_mysql.fetch_row(res)) {
          // BA object.
          uint32_t ba_id(res.value_as_u32(0));
          bas[ba_id] = ba(ba_id,                // ID.
                          res.value_as_str(1),  // Name.
                          static_cast<configuration::ba::state_source>(
                              res.value_as_u32(2)),  // State source.
                          res.value_as_f32(3),       // Warning level.
                          res.value_as_f32(4),       // Critical level.
                          static_cast<configuration::ba::downtime_behaviour>(
                              res.value_as_u32(8)));  // Downtime inheritance.

          // BA state.
          if (!res.value_is_null(5)) {
            ba_event e;
            e.ba_id = ba_id;
            e.start_time = res.value_as_u64(5);
            e.status = res.value_as_i32(6);
            e.in_downtime = res.value_as_bool(7);
            bas[ba_id].set_opened_event(e);
          }
        }
      } catch (std::exception const& e) {
        throw exceptions::msg() << "BAM: " << e.what();
      }
    }
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve BA configuration from DB: " << e.what();
  }

  // Load host_id/service_id of virtual BA services. All the associated
  // services have for description 'ba_[id]'.
  try {
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT h.host_name, s.service_description,"
        "       hsr.host_host_id, hsr.service_service_id"
        "  FROM service AS s"
        "  INNER JOIN host_service_relation AS hsr"
        "    ON s.service_id=hsr.service_service_id"
        "  INNER JOIN host AS h"
        "    ON hsr.host_host_id=h.host_id"
        "  WHERE s.service_description LIKE 'ba_%'",
        &promise);
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      uint32_t host_id = res.value_as_u32(2);
      uint32_t service_id = res.value_as_u32(3);
      std::string service_description = res.value_as_str(1);
      service_description.erase(0, strlen("ba_"));

      if (!service_description.empty()) {
        try {
          uint32_t ba_id = std::stoul(service_description);
          state::bas::iterator found = bas.find(ba_id);
          if (found == bas.end()) {
            logging::info(logging::medium)
                << "BAM: virtual BA service '" << res.value_as_str(1)
                << "' of host '" << res.value_as_str(0)
                << "' references an unknown BA (" << ba_id << ")";
            continue;
          }
          found->second.set_host_id(host_id);
          found->second.set_service_id(service_id);
          mapping.set(ba_id, res.value_as_str(0), res.value_as_str(1));
        } catch (std::exception const& e) {
          logging::info(logging::medium)
              << "BAM: service '" << res.value_as_str(1) << "' of host '"
              << res.value_as_str(0) << "' is not a valid virtual BA service";
          continue;
        }
      }
    }
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve BA service IDs from DB: " << e.what();
  }

  // Test for BA without service ID.
  for (state::bas::const_iterator it(bas.begin()), end(bas.end()); it != end;
       ++it)
    if (it->second.get_service_id() == 0)
      throw reader_exception()
          << "BAM: BA " << it->second.get_id() << " has no associated service";

  return;
}

/**
 *  Load boolean expressions from the DB.
 *
 *  @param[out] bool_exps The list of bool expression in database.
 */
void reader_v2::_load(state::bool_exps& bool_exps) {
  // Load boolean expressions themselves.
  try {
    std::string query(
        fmt::format("SELECT b.boolean_id, b.name, b.expression, b.bool_state"
                    "  FROM mod_bam_boolean AS b"
                    "  INNER JOIN mod_bam_kpi AS k"
                    "    ON b.boolean_id=k.boolean_id"
                    "  INNER JOIN mod_bam_poller_relations AS pr"
                    "    ON k.id_ba=pr.ba_id"
                    "  WHERE b.activate=1"
                    "    AND pr.poller_id={}",
                    config::applier::state::instance().poller_id()));
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      bool_exps[res.value_as_u32(0)] =
          bool_expression(res.value_as_u32(0),    // ID.
                          res.value_as_str(1),    // Name.
                          res.value_as_str(2),    // Expression.
                          res.value_as_bool(3));  // Impact if.
    }
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception() << "BAM: could not retrieve boolean expression "
                             << "configuration from DB: " << e.what();
  }
}

/**
 *  Load meta-services from the DB.
 *
 *  @param[out] meta_services  Meta-services.
 */
void reader_v2::_load(state::meta_services& meta_services) {
  // Load meta-services.
  try {
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT meta_id, meta_name, calcul_type, warning, critical,"
        "       meta_select_mode, regexp_str, metric"
        "  FROM meta_service"
        "  WHERE meta_activate='1'",
        &promise);
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      uint32_t meta_id(res.value_as_u32(0));
      meta_services[meta_id] = meta_service(
          res.value_as_u32(0), res.value_as_str(1), res.value_as_str(2),
          res.value_as_f64(3), res.value_as_f64(4),
          (res.value_as_i32(5) == 2 ? res.value_as_str(6) : ""),
          (res.value_as_i32(5) == 2 ? res.value_as_str(7) : ""));
    }
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve meta-services: " << e.what();
  }

  // Load host_id/service_id of virtual meta-service services. All
  // associated services have for description 'meta_[id]'.
  try {
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT h.host_name, s.service_description"
        "  FROM service AS s"
        "  INNER JOIN host_service_relation AS hsr"
        "    ON s.service_id=hsr.service_service_id"
        "  INNER JOIN host AS h"
        "    ON hsr.host_host_id=h.host_id"
        "  WHERE s.service_description LIKE 'meta_%'",
        &promise);
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res)) {
      std::string service_description(res.value_as_str(1));
      service_description.erase(0, strlen("meta_"));
      try {
        uint32_t meta_id(std::stoul(service_description));
        state::meta_services::iterator found(meta_services.find(meta_id));
        if (found == meta_services.end()) {
          logging::info(logging::medium)
              << "BAM: virtual meta-service service '" << res.value_as_str(1)
              << "' of host '" << res.value_as_str(0)
              << "' references an unknown meta-service (" << meta_id << ")";
          continue;
        }
      } catch (std::exception const& e) {
        logging::info(logging::medium)
            << "BAM: service '" << res.value_as_str(1) << "' of host '"
            << res.value_as_str(0)
            << "' is not a valid virtual meta-service service";
        continue;
      }
    }
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve meta-services' service IDs from DB: "
        << e.what();
  }

  // Check for meta-services without service ID.
  for (state::meta_services::const_iterator it(meta_services.begin()),
       end(meta_services.end());
       it != end; ++it) {
    // std::pair<std::string, std::string>
    //   svc(mapping.get_service(it->first));
    // if (svc.first.empty() || svc.second.empty())
    //   throw (reader_exception() << "BAM: meta-service "
    //          << it->first << " has no associated service");
  }

  // Load metrics of meta-services.
  std::unique_ptr<mysql> storage_mysql;
  for (state::meta_services::iterator it(meta_services.begin()),
       end(meta_services.end());
       it != end; ++it) {
    // SQL LIKE mode.
    if (!it->second.get_service_filter().empty() &&
        !it->second.get_metric_name().empty()) {
      std::string query(fmt::format(
          "SELECT m.metric_id"
          " FROM metrics AS m"
          " INNER JOIN index_data AS i"
          " ON m.index_id=i.id"
          " INNER JOIN services AS s"
          " ON i.host_id=s.host_id AND i.service_id=s.service_id"
          " WHERE s.description LIKE '{}'"
          " AND m.metric_name='{}'",
          it->second.get_service_filter(), it->second.get_metric_name()));
      if (!storage_mysql)
        try {
          storage_mysql.reset(new mysql(_storage_cfg));
        } catch (std::exception const& e) {
          throw reader_exception()
              << "BAM: could not initialize storage database to "
                 "retrieve metrics associated with some "
                 "meta-service: "
              << e.what();
        }
      std::promise<database::mysql_result> promise;
      storage_mysql->run_query_and_get_result(query, &promise);
      try {
        database::mysql_result res(promise.get_future().get());
        while (storage_mysql->fetch_row(res))
          it->second.add_metric(res.value_as_u32(0));
      } catch (std::exception const& e) {
        throw reader_exception()
            << "BAM: could not retrieve members of meta-service '"
            << it->second.get_name() << "': " << e.what();
      }
    }
    // Service list mode.
    else {
      try {
        std::string query(
            fmt::format("SELECT metric_id FROM meta_service_relation"
                        " WHERE meta_id={} AND activate='1'",
                        it->second.get_id()));
        std::promise<database::mysql_result> promise;
        _mysql.run_query_and_get_result(query, &promise);
        database::mysql_result res(promise.get_future().get());
        while (_mysql.fetch_row(res))
          it->second.add_metric(res.value_as_u32(0));
      } catch (reader_exception const& e) {
        (void)e;
        throw;
      } catch (std::exception const& e) {
        throw reader_exception()
            << "BAM: could not retrieve members of meta-service '"
            << it->second.get_name() << "': " << e.what();
      }
    }
  }
}

/**
 *  Load host/service IDs from the DB.
 *
 *  @param[out] mapping  Host/service mapping.
 */
void reader_v2::_load(bam::hst_svc_mapping& mapping) {
  try {
    // XXX : expand hostgroups and servicegroups
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT h.host_id, s.service_id, h.host_name, s.service_description,"
        "   service_activate"
        "  FROM service AS s"
        "  LEFT JOIN host_service_relation AS hsr"
        "    ON s.service_id=hsr.service_service_id"
        "  LEFT JOIN host AS h"
        "    ON hsr.host_host_id=h.host_id",
        &promise);
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res))
      mapping.set_service(res.value_as_str(2), res.value_as_str(3),
                          res.value_as_u32(0), res.value_as_u32(1),
                          res.value_as_str(4) == "1");
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve host/service IDs: " << e.what();
  }

  try {
    std::string query(
        "SELECT m.metric_id, m.metric_name,"
        " i.host_id,"
        " s.service_id"
        " FROM metrics AS m"
        " INNER JOIN index_data AS i"
        " ON m.index_id=i.id"
        " INNER JOIN services AS s"
        " ON i.host_id=s.host_id AND i.service_id=s.service_id");
    mysql storage_mysql(_storage_cfg);
    std::promise<database::mysql_result> promise;
    storage_mysql.run_query_and_get_result(query, &promise);
    database::mysql_result res(promise.get_future().get());
    while (storage_mysql.fetch_row(res)) {
      mapping.register_metric(res.value_as_u32(0), res.value_as_str(1),
                              res.value_as_u32(2), res.value_as_u32(3));
    }
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not retrieve known metrics: " << e.what();
  }
}

/**
 *  Load the dimensions from the database.
 */
void reader_v2::_load_dimensions() {
  std::unique_ptr<io::stream> out(new multiplexing::publisher);
  // As this operation is destructive (it truncates the database),
  // we cache the data until we are sure we have all the data
  // needed from the database.
  std::vector<std::shared_ptr<io::data> > datas;
  std::shared_ptr<dimension_truncate_table_signal> dtts(
      new dimension_truncate_table_signal);
  dtts->update_started = true;
  datas.push_back(dtts);

  // Load the dimensions.
  std::map<uint32_t, time::timeperiod::ptr> timeperiods;
  std::map<uint32_t, std::shared_ptr<dimension_ba_event> > bas;
  try {
    // Load the timeperiods themselves.
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT tp_id, tp_name, tp_alias, tp_sunday, tp_monday, tp_tuesday, "
        "tp_wednesday, tp_thursday, tp_friday, tp_saturday"
        "  FROM timeperiod",
        &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        timeperiods[res.value_as_u32(0)] = time::timeperiod::ptr(
            new time::timeperiod(res.value_as_u32(0),    // id
                                 res.value_as_str(1),    // name
                                 res.value_as_str(2),    // alias
                                 res.value_as_str(3),    // sunday
                                 res.value_as_str(4),    // monday
                                 res.value_as_str(5),    // tuesday
                                 res.value_as_str(6),    // wednesday
                                 res.value_as_str(7),    // thursday
                                 res.value_as_str(8),    // friday
                                 res.value_as_str(9)));  // saturday
        std::shared_ptr<dimension_timeperiod> tp(new dimension_timeperiod);
        tp->id = res.value_as_u32(0);
        tp->name = res.value_as_str(1);
        tp->sunday = res.value_as_str(3);
        tp->monday = res.value_as_str(4);
        tp->tuesday = res.value_as_str(5);
        tp->wednesday = res.value_as_str(6);
        tp->thursday = res.value_as_str(7);
        tp->friday = res.value_as_str(8);
        tp->saturday = res.value_as_str(9);
        datas.push_back(std::static_pointer_cast<io::data>(tp));
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "could not load timeperiods from the database: " << e.what();
    }

    // Load the BAs.
    std::string query(
        fmt::format("SELECT b.ba_id, b.name, b.description,"
                    "  b.sla_month_percent_warn, b.sla_month_percent_crit,"
                    "  b.sla_month_duration_warn,"
                    "  b.sla_month_duration_crit, b.id_reporting_period"
                    "  FROM mod_bam AS b"
                    "  INNER JOIN mod_bam_poller_relations AS pr"
                    "  ON b.ba_id=pr.ba_id"
                    "  WHERE b.activate='1'"
                    "  AND pr.poller_id={}",
                    config::applier::state::instance().poller_id()));
    promise = std::promise<database::mysql_result>();
    _mysql.run_query_and_get_result(query, &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        std::shared_ptr<dimension_ba_event> ba(new dimension_ba_event);
        ba->ba_id = res.value_as_u32(0);
        ba->ba_name = res.value_as_str(1);
        ba->ba_description = res.value_as_str(2);
        ba->sla_month_percent_warn = res.value_as_f64(3);
        ba->sla_month_percent_crit = res.value_as_f64(4);
        ba->sla_duration_warn = res.value_as_i32(5);
        ba->sla_duration_crit = res.value_as_i32(6);
        datas.push_back(std::static_pointer_cast<io::data>(ba));
        bas[ba->ba_id] = ba;
        if (!res.value_is_null(7)) {
          std::shared_ptr<dimension_ba_timeperiod_relation> dbtr(
              new dimension_ba_timeperiod_relation);
          dbtr->ba_id = res.value_as_u32(0);
          dbtr->timeperiod_id = res.value_as_u32(7);
          dbtr->is_default = true;
          datas.push_back(dbtr);
        }
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "could not retrieve BAs from the database" << e.what();
    }
    // Load the BVs.
    promise = std::promise<database::mysql_result>();
    _mysql.run_query_and_get_result(
        "SELECT id_ba_group, ba_group_name, ba_group_description"
        "  FROM mod_bam_ba_groups",
        &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        std::shared_ptr<dimension_bv_event> bv(new dimension_bv_event);
        bv->bv_id = res.value_as_u32(0);
        bv->bv_name = res.value_as_str(1);
        bv->bv_description = res.value_as_str(2);
        datas.push_back(std::static_pointer_cast<io::data>(bv));
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "could not retrieve BVs from the database: " << e.what();
    }
    // Load the BA BV relations.
    {
      std::string query(
          fmt::format("SELECT id_ba, id_ba_group"
                      " FROM mod_bam_bagroup_ba_relation as r"
                      " INNER JOIN mod_bam AS b"
                      " ON b.ba_id = r.id_ba"
                      " INNER JOIN mod_bam_poller_relations AS pr"
                      " ON b.ba_id=pr.ba_id"
                      " WHERE b.activate='1'"
                      " AND pr.poller_id={}",
                      config::applier::state::instance().poller_id()));
      std::promise<database::mysql_result> promise;
      _mysql.run_query_and_get_result(query, &promise);
      try {
        database::mysql_result res(promise.get_future().get());
        while (_mysql.fetch_row(res)) {
          std::shared_ptr<dimension_ba_bv_relation_event> babv(
              new dimension_ba_bv_relation_event);
          babv->ba_id = res.value_as_u32(0);
          babv->bv_id = res.value_as_u32(1);
          datas.push_back(std::static_pointer_cast<io::data>(babv));
        }
      } catch (std::exception const& e) {
        throw exceptions::msg()
            << "could not retrieve BV memberships of BAs: " << e.what();
      }
    }

    // Load the KPIs
    // Unfortunately, we need to get the names of the
    // service/host/meta_service/ba/boolean expression associated with
    // this KPI. This explains the numerous joins.
    {
      std::string query(
          fmt::format("SELECT k.kpi_id, k.kpi_type, k.host_id, k.service_id,"
                      "       k.id_ba, k.id_indicator_ba, k.meta_id,"
                      "       k.boolean_id,"
                      "       COALESCE(COALESCE(k.drop_warning, ww.impact), "
                      "g.average_impact),"
                      "       COALESCE(COALESCE(k.drop_critical, cc.impact), "
                      "g.average_impact),"
                      "       COALESCE(COALESCE(k.drop_unknown, uu.impact), "
                      "g.average_impact),"
                      "       h.host_name, s.service_description, b.name,"
                      "       meta.meta_name, boo.name"
                      "  FROM mod_bam_kpi AS k"
                      "  LEFT JOIN mod_bam_impacts AS ww"
                      "    ON k.drop_warning_impact_id = ww.id_impact"
                      "  LEFT JOIN mod_bam_impacts AS cc"
                      "    ON k.drop_critical_impact_id = cc.id_impact"
                      "  LEFT JOIN mod_bam_impacts AS uu"
                      "    ON k.drop_unknown_impact_id = uu.id_impact"
                      "  LEFT JOIN host AS h"
                      "    ON h.host_id = k.host_id"
                      "  LEFT JOIN service AS s"
                      "    ON s.service_id = k.service_id"
                      "  INNER JOIN mod_bam AS b"
                      "    ON b.ba_id = k.id_ba"
                      "  INNER JOIN mod_bam_poller_relations AS pr"
                      "    ON b.ba_id = pr.ba_id"
                      "  LEFT JOIN meta_service AS meta"
                      "    ON meta.meta_id = k.meta_id"
                      "  LEFT JOIN mod_bam_boolean as boo"
                      "    ON boo.boolean_id = k.boolean_id"
                      "  LEFT JOIN (SELECT id_ba, 100.0 / COUNT(kpi_id) AS "
                      "average_impact"
                      "               FROM mod_bam_kpi"
                      "               WHERE activate='1'"
                      "               GROUP BY id_ba) AS g"
                      "   ON k.id_ba=g.id_ba"
                      "  WHERE k.activate='1'"
                      "    AND b.activate='1'"
                      "    AND pr.poller_id={}",
                      config::applier::state::instance().poller_id()));
      std::promise<database::mysql_result> promise;
      _mysql.run_query_and_get_result(query, &promise);

      try {
        database::mysql_result res(promise.get_future().get());

        while (_mysql.fetch_row(res)) {
          std::shared_ptr<dimension_kpi_event> k(new dimension_kpi_event);
          k->kpi_id = res.value_as_u32(0);
          k->host_id = res.value_as_u32(2);
          k->service_id = res.value_as_u32(3);
          k->ba_id = res.value_as_u32(4);
          k->kpi_ba_id = res.value_as_u32(5);
          k->meta_service_id = res.value_as_u32(6);
          k->boolean_id = res.value_as_u32(7);
          k->impact_warning = res.value_as_f64(8);
          k->impact_critical = res.value_as_f64(9);
          k->impact_unknown = res.value_as_f64(10);
          k->host_name = res.value_as_str(11);
          k->service_description = res.value_as_str(12);
          k->ba_name = res.value_as_str(13);
          k->meta_service_name = res.value_as_str(14);
          k->boolean_name = res.value_as_str(15);

          // Resolve the id_indicator_ba.
          if (k->kpi_ba_id) {
            std::map<uint32_t,
                     std::shared_ptr<dimension_ba_event> >::const_iterator
                found = bas.find(k->kpi_ba_id);
            if (found == bas.end()) {
              logging::error(logging::high)
                  << "BAM: could not retrieve BA " << k->kpi_ba_id
                  << " used as KPI " << k->kpi_id
                  << " in dimension table: ignoring this KPI";
              continue;
            }
            k->kpi_ba_name = found->second->ba_name;
          }
          datas.push_back(std::static_pointer_cast<io::data>(k));
        }
      } catch (std::exception const& e) {
        throw exceptions::msg()
            << "could not retrieve KPI dimensions: " << e.what();
      }
    }

    // Load the ba-timeperiods relations.
    promise = std::promise<database::mysql_result>();
    _mysql.run_query_and_get_result(
        "SELECT ba_id, tp_id FROM mod_bam_relations_ba_timeperiods", &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        std::shared_ptr<dimension_ba_timeperiod_relation> dbtr(
            new dimension_ba_timeperiod_relation);
        dbtr->ba_id = res.value_as_u32(0);
        dbtr->timeperiod_id = res.value_as_u32(1);
        dbtr->is_default = false;
        datas.push_back(dbtr);
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "could not retrieve the timeperiods associated with the BAs: "
          << e.what();
    }

    // End the update.
    dtts = std::shared_ptr<dimension_truncate_table_signal>(
        new dimension_truncate_table_signal);
    dtts->update_started = false;
    datas.push_back(dtts);

    // Write all the cached data to the publisher.
    for (std::vector<std::shared_ptr<io::data> >::iterator it(datas.begin()),
         end(datas.end());
         it != end; ++it)
      out->write(*it);
  } catch (reader_exception const& e) {
    (void)e;
    throw;
  } catch (std::exception const& e) {
    throw reader_exception()
        << "BAM: could not load some dimension table: " << e.what();
  }
}
