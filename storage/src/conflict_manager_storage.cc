/*
** Copyright 2019 Centreon
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

#include <fmt/format.h>

#include <cfloat>
#include <cstring>
#include <list>
#include <sstream>

#include "com/centreon/broker/database/table_max_size.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

#define BAM_NAME "_Module_"

/**
 *  Check that the floating point values are the same number or are NaN or are
 *  INFINITY at the same time. The idea is to check if a is changed into b, did
 *  it really change?
 *
 *  @param[in] a Floating point value.
 *  @param[in] b Floating point value.
 *
 *  @return true if they are equal, false otherwise.
 */
static inline bool check_equality(double a, double b) {
  static const double eps = 0.000001;
  if (a == b)
    return true;
  if (std::isnan(a) && std::isnan(b))
    return true;
  if (fabs(a - b) < eps)
    return true;
  return false;
}
/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 *
 * @return the number of events sent to the database.
 */
void conflict_manager::_storage_process_service_status(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t) {
  auto& d = std::get<0>(t);
  neb::service_status const& ss{*static_cast<neb::service_status*>(d.get())};
  uint64_t host_id = ss.host_id, service_id = ss.service_id;
  log_v2::perfdata()->debug(
      "conflict_manager::_storage_process_service_status(): host_id:{}, "
      "service_id:{}",
      host_id, service_id);
  auto it_index_cache = _index_cache.find({host_id, service_id});
  uint64_t index_id;
  uint32_t rrd_len;
  int32_t conn =
      _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]);
  bool index_locked{false};
  bool special{!strncmp(ss.host_name.c_str(), BAM_NAME, sizeof(BAM_NAME) - 1)};

  auto add_metric_in_cache =
      [this](uint64_t index_id, uint64_t host_id, uint64_t service_id,
             neb::service_status const& ss, bool index_locked, bool special,
             uint32_t& rrd_len) -> void {
    if (index_id == 0) {
      throw msg_fmt(
          "storage: could not fetch index_id of newly inserted index ({}"
          ", {})", host_id, service_id);
    }

    /* Insert index in cache. */
    log_v2::perfdata()->info(
        "conflict_manager: add_metric_in_cache: index {}, for host_id {} and "
        "service_id {}", index_id, host_id, service_id);
    index_info info{.host_name = ss.host_name,
                    .index_id = index_id,
                    .locked = index_locked,
                    .rrd_retention = _rrd_len,
                    .service_description = ss.service_description,
                    .special = special};

    _index_cache[{host_id, service_id}] = std::move(info);
    rrd_len = _rrd_len;
    log_v2::perfdata()->debug(
        "conflict_manager:: add_metric_in_cache: returned rrd_len {}", rrd_len);

    /* Create the metric mapping. */
    std::shared_ptr<storage::index_mapping> im{
        std::make_shared<storage::index_mapping>(index_id, host_id,
                                                 service_id)};
    multiplexing::publisher pblshr;
    pblshr.write(im);
  };

  /* Index does not exist */
  if (it_index_cache == _index_cache.end()) {
    _finish_action(-1, actions::index_data);
    log_v2::perfdata()->debug(
        "conflict_manager::_storage_process_service_status(): host_id:{}, "
        "service_id:{} - index not found in cache",
        host_id, service_id);

    if (!_index_data_insert.prepared())
      _index_data_insert = _mysql.prepare_query(
          "INSERT INTO index_data "
          "(host_id,host_name,service_id,service_description,must_be_rebuild,"
          "special) VALUES (?,?,?,?,?,?)");

    fmt::string_view hv(misc::string::truncate(
        ss.host_name, get_index_data_col_size(index_data_host_name)));
    fmt::string_view sv(misc::string::truncate(
        ss.service_description,
        get_index_data_col_size(index_data_service_description)));
    _index_data_insert.bind_value_as_i32(0, host_id);
    _index_data_insert.bind_value_as_str(1, hv);
    _index_data_insert.bind_value_as_i32(2, service_id);
    _index_data_insert.bind_value_as_str(3, sv);
    _index_data_insert.bind_value_as_str(4, "0");
    _index_data_insert.bind_value_as_str(5, special ? "1" : "0");
    std::promise<uint64_t> promise;
    _mysql.run_statement_and_get_int<uint64_t>(
        _index_data_insert, &promise, database::mysql_task::LAST_INSERT_ID,
        conn);
    try {
      index_id = promise.get_future().get();
      add_metric_in_cache(index_id, host_id, service_id, ss, index_locked,
                          special, rrd_len);
    } catch (std::exception const& e) {
      try {
        if (!_index_data_query.prepared())
          _index_data_query = _mysql.prepare_query(
              "SELECT id from index_data WHERE host_id=? AND service_id=?");

        _index_data_query.bind_value_as_i32(0, host_id);
        _index_data_query.bind_value_as_i32(1, service_id);
        {
          std::promise<database::mysql_result> promise;
          log_v2::sql()->debug(
              "Query for index_data for host_id={} and service_id={}", host_id,
              service_id);
          _mysql.run_statement_and_get_result(_index_data_query, &promise,
                                              conn);

          database::mysql_result res(promise.get_future().get());
          if (_mysql.fetch_row(res))
            index_id = res.value_as_u64(0);
          else
            index_id = 0;
        }

        if (index_id == 0)
          throw msg_fmt(
              "storage: could not fetch index_id of newly inserted index ({}, "
              "{})",
              host_id, service_id);

        if (!_index_data_update.prepared())
          _index_data_update = _mysql.prepare_query(
              "UPDATE index_data "
              "SET host_name=?, service_description=?, must_be_rebuild=?, "
              "special=? "
              "WHERE id=?");

        log_v2::sql()->debug(
            "Updating index_data for host_id={} and service_id={}", host_id,
            service_id);
        _index_data_update.bind_value_as_str(0, hv);
        _index_data_update.bind_value_as_str(1, sv);
        _index_data_update.bind_value_as_str(2, "0");
        _index_data_update.bind_value_as_str(3, special ? "1" : "0");
        _index_data_update.bind_value_as_u64(4, index_id);
        {
          std::promise<database::mysql_result> promise;
          _mysql.run_statement_and_get_result(_index_data_update, &promise,
                                              conn);
          promise.get_future().get();
        }

        add_metric_in_cache(index_id, host_id, service_id, ss, index_locked,
                            special, rrd_len);
        log_v2::sql()->debug(
            "Index {} stored in cache for host_id={} and service_id={}",
            index_id, host_id, service_id);
      } catch (std::exception const& e) {
        throw msg_fmt(
            "storage: insertion of index ( {}, {}"
            ") failed: {}", host_id, service_id, e.what());
      }
    }
  } else {
    index_id = it_index_cache->second.index_id;
    rrd_len = it_index_cache->second.rrd_retention;
    index_locked = it_index_cache->second.locked;
    log_v2::perfdata()->debug(
        "conflict_manager: host_id:{}, service_id:{} - index already in cache "
        "- index_id {}, rrd_len {}",
        host_id, service_id, index_id, rrd_len);
  }

  if (index_id) {
    /* Generate status event */
    log_v2::perfdata()->debug(
        "conflict_manager: host_id:{}, service_id:{} - generating status event "
        "with index_id {}, rrd_len: {}",
        host_id, service_id, index_id, rrd_len);
    std::shared_ptr<storage::status> status(std::make_shared<storage::status>(
        ss.last_check, index_id,
        static_cast<uint32_t>(ss.check_interval * _interval_length), false,
        rrd_len, ss.last_hard_state));
    multiplexing::publisher().write(status);

    if (!ss.perf_data.empty()) {
      /* Statements preparations */
      if (!_metrics_insert.prepared()) {
        _metrics_insert = _mysql.prepare_query(
            "INSERT INTO metrics "
            "(index_id,metric_name,unit_name,warn,warn_low,"
            "warn_threshold_mode,crit,"
            "crit_low,crit_threshold_mode,min,max,current_value,"
            "data_source_type) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");
      }

      /* Parse perfdata. */
      std::list<storage::perfdata> pds;
      storage::parser p;
      try {
        _finish_action(-1, actions::metrics);
        p.parse_perfdata(ss.host_id, ss.service_id, ss.perf_data.c_str(), pds);

        std::list<std::shared_ptr<io::data>> to_publish;
        for (auto& pd : pds) {
          auto it_index_cache = _metric_cache.find({index_id, pd.name()});

          /* The cache does not contain this metric */
          uint32_t metric_id;
          bool need_metric_mapping = true;
          if (it_index_cache == _metric_cache.end()) {
            log_v2::perfdata()->debug(
                "conflict_manager: no metrics corresponding to index {} and "
                "perfdata '{}' found in cache",
                index_id, pd.name());
            /* Let's insert it */
            _metrics_insert.bind_value_as_u64(0, index_id);
            _metrics_insert.bind_value_as_str(1, pd.name());
            _metrics_insert.bind_value_as_str(2, pd.unit());
            _metrics_insert.bind_value_as_f32(3, pd.warning());
            _metrics_insert.bind_value_as_f32(4, pd.warning_low());
            _metrics_insert.bind_value_as_tiny(5, pd.warning_mode());
            _metrics_insert.bind_value_as_f32(6, pd.critical());
            _metrics_insert.bind_value_as_f32(7, pd.critical_low());
            _metrics_insert.bind_value_as_tiny(8, pd.critical_mode());
            _metrics_insert.bind_value_as_f32(9, pd.min());
            _metrics_insert.bind_value_as_f32(10, pd.max());
            _metrics_insert.bind_value_as_f32(11, pd.value());

            uint32_t type = pd.value_type();
            char t[2];
            t[0] = '0' + type;
            t[1] = 0;
            _metrics_insert.bind_value_as_str(12, t);

            // Execute query.
            std::promise<int> promise;
            _mysql.run_statement_and_get_int<int>(
                _metrics_insert, &promise, database::mysql_task::LAST_INSERT_ID,
                conn);
            try {
              metric_id = promise.get_future().get();

              // Insert metric in cache.
              log_v2::perfdata()->info(
                  "conflict_manager: new metric {} for index {} and perfdata "
                  "'{}'",
                  metric_id, index_id, pd.name());
              metric_info info{.locked = false,
                               .metric_id = metric_id,
                               .type = type,
                               .value = pd.value(),
                               .unit_name = pd.unit(),
                               .warn = pd.warning(),
                               .warn_low = pd.warning_low(),
                               .warn_mode = pd.warning_mode(),
                               .crit = pd.critical(),
                               .crit_low = pd.critical_low(),
                               .crit_mode = pd.critical_mode(),
                               .min = pd.min(),
                               .max = pd.max(),
                               .metric_mapping_sent =
                                   true};  // It will be done after this block

              std::lock_guard<std::mutex> lock(_metric_cache_m);
              _metric_cache[{index_id, pd.name()}] = info;
            } catch (std::exception const& e) {
              log_v2::perfdata()->error(
                  "conflict_manager: failed to create metric {} with type {}, "
                  "value {}, unit_name {}, warn {}, warn_low {}, warn_mode {}, "
                  "crit {}, crit_low {}, crit_mode {}, min {} and max {}",
                  metric_id, type, pd.value(), pd.unit(), pd.warning(),
                  pd.warning_low(), pd.warning_mode(), pd.critical(),
                  pd.critical_low(), pd.critical_mode(), pd.min(), pd.max());
              throw msg_fmt(
                  "storage: insertion of metric '{}"
                  "' of index {} failed: {}",
                  pd.name(), index_id, e.what());
            }
          } else {
            std::lock_guard<std::mutex> lock(_metric_cache_m);
            /* We have the metric in the cache */
            metric_id = it_index_cache->second.metric_id;
            if (!it_index_cache->second.metric_mapping_sent)
              it_index_cache->second.metric_mapping_sent = true;
            else
              need_metric_mapping = false;

            pd.value_type(
                static_cast<perfdata::data_type>(it_index_cache->second.type));

            log_v2::perfdata()->debug(
                "conflict_manager: metric {} concerning index {}, perfdata "
                "'{}' found in cache",
                it_index_cache->second.metric_id, index_id, pd.name());
            // Should we update metrics ?
            if (!check_equality(it_index_cache->second.value, pd.value()) ||
                it_index_cache->second.unit_name != pd.unit() ||
                !check_equality(it_index_cache->second.warn, pd.warning()) ||
                !check_equality(it_index_cache->second.warn_low,
                                pd.warning_low()) ||
                it_index_cache->second.warn_mode != pd.warning_mode() ||
                !check_equality(it_index_cache->second.crit, pd.critical()) ||
                !check_equality(it_index_cache->second.crit_low,
                                pd.critical_low()) ||
                it_index_cache->second.crit_mode != pd.critical_mode() ||
                !check_equality(it_index_cache->second.min, pd.min()) ||
                !check_equality(it_index_cache->second.max, pd.max())) {
              log_v2::perfdata()->info(
                  "conflict_manager: updating metric {} of index {}, perfdata "
                  "'{}' with unit: {}, warning: {}:{}, critical: {}:{}, min: "
                  "{}, max: {}",
                  it_index_cache->second.metric_id, index_id, pd.name(),
                  pd.unit(), pd.warning_low(), pd.warning(), pd.critical_low(),
                  pd.critical(), pd.min(), pd.max());
              // Update metrics table.
              it_index_cache->second.unit_name = pd.unit();
              it_index_cache->second.value = pd.value();
              it_index_cache->second.warn = pd.warning();
              it_index_cache->second.warn_low = pd.warning_low();
              it_index_cache->second.crit = pd.critical();
              it_index_cache->second.crit_low = pd.critical_low();
              it_index_cache->second.warn_mode = pd.warning_mode();
              it_index_cache->second.crit_mode = pd.critical_mode();
              it_index_cache->second.min = pd.min();
              it_index_cache->second.max = pd.max();
              _metrics[it_index_cache->second.metric_id] =
                  &it_index_cache->second;
            }
          }
          if (need_metric_mapping)
            to_publish.emplace_back(
                std::make_shared<storage::metric_mapping>(index_id, metric_id));

          if (_store_in_db) {
            // Append perfdata to queue.
            metric_value val;
            val.c_time = ss.last_check;
            val.metric_id = metric_id;
            val.status = ss.current_state;
            val.value = pd.value();
            _perfdata_queue.push_back(val);
          }

          // Send perfdata event to processing.
          if (!index_locked) {
            std::shared_ptr<storage::metric> perf{
                std::make_shared<storage::metric>(
                    ss.host_id, ss.service_id, pd.name(), ss.last_check,
                    static_cast<uint32_t>(ss.check_interval * _interval_length),
                    false, metric_id, rrd_len, pd.value(), pd.value_type())};
            log_v2::perfdata()->debug(
                "conflict_manager: generating perfdata event for metric {} "
                "(name '{}', ctime {}, value {}, rrd_len {}, data_type {})",
                perf->metric_id, perf->name, perf->ctime, perf->value, rrd_len,
                perf->value_type);
            multiplexing::publisher().write(perf);
          }
        }
        multiplexing::publisher pblshr;
        pblshr.write(to_publish);
      } catch (storage::exceptions::perfdata const& e) {
        logging::error(logging::medium)
            << "storage: error while parsing perfdata of service (" << host_id
            << ", " << service_id << "): " << e.what();
      }
    }
  }
  *std::get<2>(t) = true;
}

void conflict_manager::_update_metrics() {
  if (_metrics.empty())
    return;
  std::deque<std::string> m;
  for (auto it = _metrics.begin(); it != _metrics.end(); ++it) {
    metric_info* metric = it->second;
    m.emplace_back(fmt::format(
        "({},'{}',{},{},'{}',{},{},'{}',{},{},{})", metric->metric_id,
        misc::string::escape(metric->unit_name,
                             get_metrics_col_size(metrics_unit_name)),
        std::isnan(metric->warn) || std::isinf(metric->warn)
            ? "NULL"
            : fmt::format("{}", metric->warn),
        std::isnan(metric->warn_low) || std::isinf(metric->warn_low)
            ? "NULL"
            : fmt::format("{}", metric->warn_low),
        metric->warn_mode ? "1" : "0",
        std::isnan(metric->crit) || std::isinf(metric->crit)
            ? "NULL"
            : fmt::format("{}", metric->crit),
        std::isnan(metric->crit_low) || std::isinf(metric->crit_low)
            ? "NULL"
            : fmt::format("{}", metric->crit_low),
        metric->crit_mode ? "1" : "0",
        std::isnan(metric->min) || std::isinf(metric->min)
            ? "NULL"
            : fmt::format("{}", metric->min),
        std::isnan(metric->max) || std::isinf(metric->max)
            ? "NULL"
            : fmt::format("{}", metric->max),
        std::isnan(metric->value) || std::isinf(metric->value)
            ? "NULL"
            : fmt::format("{}", metric->value)));
  }
  std::string query(fmt::format(
      "INSERT INTO metrics (metric_id, unit_name, warn, warn_low, "
      "warn_threshold_mode, crit, crit_low, crit_threshold_mode, min, max, "
      "current_value) VALUES {} ON DUPLICATE KEY UPDATE "
      "unit_name=VALUES(unit_name), warn=VALUES(warn), "
      "warn_low=VALUES(warn_low), "
      "warn_threshold_mode=VALUES(warn_threshold_mode), crit=VALUES(crit), "
      "crit_low=VALUES(crit_low), "
      "crit_threshold_mode=VALUES(crit_threshold_mode), min=VALUES(min), "
      "max=VALUES(max), current_value=VALUES(current_value)",
      fmt::join(m, ",")));
  int32_t conn = _mysql.choose_best_connection(-1);
  _finish_action(-1, actions::metrics);
  log_v2::sql()->trace("Send query: {}", query);
  _mysql.run_query(query, database::mysql_error::update_metrics, false, conn);
  _add_action(conn, actions::metrics);
  _metrics.clear();
}

/**
 *  Insert performance data entries in the data_bin table.
 */
void conflict_manager::_insert_perfdatas() {
  if (!_perfdata_queue.empty()) {
    // Status.
    //_update_status("status=inserting performance data\n");

    uint32_t count = 0;

    // Insert first entry.
    std::ostringstream query;
    {
      metric_value& mv(_perfdata_queue.front());
      query.precision(10);
      query << std::scientific
            << "INSERT INTO data_bin (id_metric,ctime,status,value) VALUES ("
            << mv.metric_id << "," << mv.c_time << ",'" << mv.status << "',";
      if (std::isinf(mv.value))
        query << ((mv.value < 0.0) ? -FLT_MAX : FLT_MAX);
      else if (std::isnan(mv.value))
        query << "NULL";
      else
        query << mv.value;
      query << ")";
      _perfdata_queue.pop_front();
      count++;
    }

    // Insert perfdata in data_bin.
    while (!_perfdata_queue.empty()) {
      metric_value& mv(_perfdata_queue.front());
      query << ",(" << mv.metric_id << "," << mv.c_time << ",'" << mv.status
            << "',";
      if (std::isinf(mv.value))
        query << ((mv.value < 0.0) ? -FLT_MAX : FLT_MAX);
      else if (std::isnan(mv.value))
        query << "NULL";
      else
        query << mv.value;
      query << ")";
      _perfdata_queue.pop_front();
      count++;
    }

    // Execute query.
    _mysql.run_query(query.str(), database::mysql_error::insert_data);

    //_update_status("");
    log_v2::sql()->info("storage: {} perfdata inserted in data_bin", count);
  }
}

/**
 *  Check for deleted index.
 */
void conflict_manager::_check_deleted_index() {
  // Info.
  logging::info(logging::medium) << "storage: starting DB cleanup";
  uint32_t deleted_index(0);
  uint32_t deleted_metrics(0);
  //_update_status("status=deleting old performance data (might take a
  // while)\n");

  // Fetch next index to delete.
  {
    std::promise<database::mysql_result> promise;
    int32_t conn = _mysql.choose_best_connection(-1);
    std::unordered_set<uint64_t> index_to_delete;
    std::list<uint64_t> metrics_to_delete;
    try {
      _mysql.run_query_and_get_result(
          "SELECT m.index_id,m.metric_id FROM metrics m LEFT JOIN index_data "
          "i ON i.id=m.index_id WHERE i.to_delete=1",
          &promise, conn);
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        index_to_delete.insert(res.value_as_u64(0));
        metrics_to_delete.push_back(res.value_as_u64(1));
      }
    } catch (std::exception const& e) {
      throw msg_fmt("could not query index table to get index to delete: {} ",
                    e.what());
    }

    // Delete metrics.

    std::string query;
    std::string err_msg;
    for (int64_t i : metrics_to_delete) {
      query = fmt::format("DELETE FROM metrics WHERE metric_id={}", i);
      _mysql.run_query(query, database::mysql_error::delete_metric, false,
                       conn);
      _add_action(conn, actions::metrics);
      deleted_metrics++;
    }

    // Delete index from DB.
    for (int64_t i : index_to_delete) {
      query = fmt::format("DELETE FROM index_data WHERE id={}", i);
      _mysql.run_query(query, database::mysql_error::delete_index, false, conn);
      _add_action(conn, actions::index_data);

      // Remove associated graph.
      std::shared_ptr<storage::remove_graph> rg{
          std::make_shared<storage::remove_graph>(i, true)};
      multiplexing::publisher().write(rg);
      deleted_index++;
    }
  }

  // End.
  log_v2::perfdata()->info(
      "storage: end of DB cleanup: {} metrics and {} indices removed",
      deleted_metrics, deleted_index);
}
