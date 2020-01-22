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

#include <cfloat>
#include <cstring>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
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

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

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
int32_t conflict_manager::_storage_process_service_status() {
  logging::debug(logging::low) << "storage: process_service_status...";

  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};
  neb::service_status const& ss{*static_cast<neb::service_status *>(d.get())};
  uint64_t host_id = ss.host_id, service_id = ss.service_id;
  auto it_index_cache = _index_cache.find({host_id, service_id});
  uint32_t index_id, rrd_len;
  int32_t conn =
    _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]);
  bool index_locked{false};
  bool special{!strncmp(ss.host_name.c_str(), BAM_NAME, sizeof(BAM_NAME) - 1)};

  auto add_metric_in_cache = [this](
    uint32_t index_id,
    uint64_t host_id,
    uint64_t service_id,
    neb::service_status const& ss,
    bool index_locked,
    bool special,
    uint32_t& rrd_len) -> void {
    if (index_id == 0) {
      throw broker::exceptions::msg()
        << "storage: could not fetch index_id of newly inserted index ("
        << host_id << ", " << service_id << ")";
    }

    /* Insert index in cache. */
    logging::info(logging::medium) << "storage: new index " << index_id
                                   << " for (" << host_id << ", "
                                   << service_id << ")";
    index_info info{.host_name = ss.host_name,
      .index_id = index_id,
      .locked = index_locked,
      .rrd_retention = _rrd_len,
      .service_description = ss.service_description,
      .special = special
    };

    _index_cache[{host_id, service_id}] = std::move(info);
    rrd_len = _rrd_len;

    /* Create the metric mapping. */
    std::shared_ptr<storage::index_mapping> im{
      std::make_shared<storage::index_mapping>(
        index_id, host_id, service_id)};
    multiplexing::publisher pblshr;
    pblshr.write(im);
  };

  /* Index does not exist */
  if (it_index_cache == _index_cache.end()) {
    _finish_action(-1, actions::index_data);
    logging::debug(logging::low) << "storage: index of (" << host_id << ", "
                                 << service_id << ") not found in cache";

    if (!_index_data_insert.prepared())
      _index_data_insert = _mysql.prepare_query(
        "INSERT INTO index_data "
        "(host_id,host_name,service_id,service_description,must_be_rebuild,"
        "special) VALUES (?,?,?,?,?,?)");

    _index_data_insert.bind_value_as_i32(0, host_id);
    _index_data_insert.bind_value_as_str(1, ss.host_name);
    _index_data_insert.bind_value_as_i32(2, service_id);
    _index_data_insert.bind_value_as_str(3, ss.service_description);
    _index_data_insert.bind_value_as_str(4, "0");
    _index_data_insert.bind_value_as_str(5, special ? "1" : "0");
    std::promise<int> promise;
    _mysql.run_statement_and_get_int(_index_data_insert,
                                     &promise,
                                     database::mysql_task::LAST_INSERT_ID,
                                     conn);
    try {
      index_id = promise.get_future().get();
      add_metric_in_cache(index_id,
                          host_id,
                          service_id,
                          ss,
                          index_locked,
                          special,
                          rrd_len);
    }
    catch (std::exception const& e) {
      try {
        if (!_index_data_query.prepared())
          _index_data_query = _mysql.prepare_query(
            "SELECT id from index_data WHERE host_id=? AND service_id=?");

        _index_data_query.bind_value_as_i32(0, host_id);
        _index_data_query.bind_value_as_i32(1, service_id);
        {
          std::promise<database::mysql_result> promise;
          _mysql.run_statement_and_get_result(_index_data_query,
                                              &promise,
                                              conn);

          database::mysql_result res(promise.get_future().get());
          if (_mysql.fetch_row(res))
            index_id = res.value_as_u32(0);
          else
            index_id = 0;
        }

        if (index_id == 0)
          throw broker::exceptions::msg()
            << "storage: could not fetch index_id of newly inserted index ("
            << host_id << ", " << service_id << ")";

        if (!_index_data_update.prepared())
          _index_data_update = _mysql.prepare_query(
            "UPDATE index_data "
            "SET host_name=?, service_description=?, must_be_rebuild=?, special=? "
            "WHERE id=?");

        _index_data_update.bind_value_as_str(0, ss.host_name);
        _index_data_update.bind_value_as_str(1, ss.service_description);
        _index_data_update.bind_value_as_str(2, "0");
        _index_data_update.bind_value_as_str(3, special ? "1" : "0");
        _index_data_update.bind_value_as_i32(4, index_id);
        {
          std::promise<database::mysql_result> promise;
          _mysql.run_statement_and_get_result(_index_data_update,
                                              &promise,
                                              conn);
          promise.get_future().get();
        }

        add_metric_in_cache(index_id,
                            host_id,
                            service_id,
                            ss,
                            index_locked,
                            special,
                            rrd_len);
      }
      catch (std::exception const& e) {
        throw broker::exceptions::msg() << "storage: insertion of index ("
                                        << host_id << ", " << service_id
                                        << ") failed: " << e.what();
      }
    }
  } else {
    index_id = it_index_cache->second.index_id;
    rrd_len = it_index_cache->second.rrd_retention;
    index_locked = it_index_cache->second.locked;
  }

  if (index_id) {
    /* Generate status event */
    logging::debug(logging::low) << "storage: generating status event for ("
                                 << host_id << ", " << service_id
                                 << ") of index " << index_id;
    std::shared_ptr<storage::status> status{std::make_shared<storage::status>(
      ss.last_check,
      index_id,
      static_cast<uint32_t>(ss.check_interval * _interval_length),
      false,
      rrd_len,
      ss.last_hard_state)};
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

        _metrics_update = _mysql.prepare_query(
          "UPDATE metrics SET "
          "unit_name=?,warn=?,warn_low=?,warn_threshold_mode=?,crit=?,"
          "crit_low=?,crit_threshold_mode=?,min=?,max=?,current_value=?"
          " WHERE metric_id=?");
      }

      /* Parse perfdata. */
      std::list<storage::perfdata> pds;
      storage::parser p;
      try {
        _finish_action(-1, actions::metrics);
        p.parse_perfdata(ss.perf_data.c_str(), pds);

        for (storage::perfdata& pd : pds) {
          auto it_index_cache = _metric_cache.find({index_id, pd.name()});

          /* The cache does not contain this metric */
          uint32_t metric_id;
          if (it_index_cache == _metric_cache.end()) {
            /* Let's insert it */
            _metrics_insert.bind_value_as_i32(0, index_id);
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

            uint32_t type = 0;  // FIXME DBR: can be one of the data_type
            // proposed in perfdata.hh
            char t[2];
            t[0] = '1' + type;
            t[1] = 0;
            _metrics_insert.bind_value_as_str(12, t);

            // Execute query.
            std::promise<int> promise;
            _mysql.run_statement_and_get_int(
              _metrics_insert,
              &promise,
              database::mysql_task::LAST_INSERT_ID,
              conn);
            try {
              metric_id = promise.get_future().get();

              // Insert metric in cache.
              logging::info(logging::medium)
                << "storage: new metric " << metric_id << " for (" << index_id
                << ", " << pd.name() << ")";
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
                .max = pd.max()};

              _metric_cache[{index_id, pd.name()}] = info;
            }
            catch (std::exception const& e) {
              throw broker::exceptions::msg()
                << "storage: insertion of metric '" << pd.name()
                << "' of index " << index_id << " failed: " << e.what();
            }
          } else {
            /* We have the metric in the cache */
            metric_id = it_index_cache->second.metric_id;

            logging::debug(logging::low) << "storage: found metric "
                                         << it_index_cache->second.metric_id
                                         << " of (" << index_id << ", "
                                         << pd.name() << ") in cache";
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
              logging::info(logging::medium)
                << "storage: updating metric "
                << it_index_cache->second.metric_id << " of (" << index_id
                << ", " << pd.name() << ") (unit: " << pd.unit()
                << ", warning: " << pd.warning_low() << ":" << pd.warning()
                << ", critical: " << pd.critical_low() << ":" << pd.critical()
                << ", min: " << pd.min() << ", max: " << pd.max() << ")";
              // Update metrics table.
              _metrics_update.bind_value_as_str(0, pd.unit());
              _metrics_update.bind_value_as_f32(1, pd.warning());
              _metrics_update.bind_value_as_f32(2, pd.warning_low());
              _metrics_update.bind_value_as_tiny(3, pd.warning_mode());
              _metrics_update.bind_value_as_f32(4, pd.critical());
              _metrics_update.bind_value_as_f32(5, pd.critical_low());
              _metrics_update.bind_value_as_tiny(6, pd.critical_mode());
              _metrics_update.bind_value_as_f32(7, pd.min());
              _metrics_update.bind_value_as_f32(8, pd.max());
              _metrics_update.bind_value_as_f32(9, pd.value());
              _metrics_update.bind_value_as_i32(
                10, it_index_cache->second.metric_id);

              // Only use the thread_id 0
              _mysql.run_statement(
                _metrics_update, "UPDATE metrics", false, conn);
            }
          }
          std::shared_ptr<storage::metric_mapping> mm =
            std::make_shared<storage::metric_mapping>(index_id, metric_id);
          multiplexing::publisher pblshr;
          pblshr.write(mm);

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
                ss.host_id,
                ss.service_id,
                pd.name(),
                ss.last_check,
                static_cast<uint32_t>(ss.check_interval * _interval_length),
                false,
                metric_id,
                rrd_len,
                pd.value(),
                pd.value_type())};
            logging::debug(logging::high)
              << "storage: generating perfdata event for metric "
              << perf->metric_id << " (name " << perf->name << ", ctime "
              << perf->ctime << ", value " << perf->value << ")";
            multiplexing::publisher().write(perf);
          }
        }
      }
      catch (storage::exceptions::perfdata const& e) {
        logging::error(logging::medium)
          << "storage: error while parsing perfdata of service (" << host_id
          << ", " << service_id << "): " << e.what();
      }
    }
  }

  _pop_event(p);
  return 1;
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
    _mysql.run_query(query.str(),
                     "storage: could not insert data in data_bin: ");

    //_update_status("");
    logging::info(logging::low) << "storage: " << count
                                << " perfdatas inserted in data_bin";
  }
}

/**
 *  Check for deleted index.
 */
void conflict_manager::_check_deleted_index() {
  // Info.
  logging::info(logging::medium) << "storage: starting DB cleanup";
  uint64_t deleted_index(0);
  uint64_t deleted_metrics(0);
  //_update_status("status=deleting old performance data (might take a while)\n");

  // Fetch next index to delete.
  uint64_t index_id;
  {
    std::promise<database::mysql_result> promise;
    int32_t conn = _mysql.choose_best_connection();
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
    }
    catch (std::exception const& e) {
      throw exceptions::msg()
        << "storage: could not query index table to get index to delete: "
        << e.what();
    }

    // Delete metrics.

    for (int64_t i : metrics_to_delete) {
      std::ostringstream oss;
      oss << "DELETE FROM metrics WHERE metric_id=" << i;
      std::ostringstream oss_error;
      oss_error << "storage: cannot delete metric " << i << ": ";
      _mysql.run_query(oss.str(), oss_error.str(), false, conn);
      _add_action(conn, actions::metrics);
    }

    // Delete index from DB.
    for (int64_t i : index_to_delete) {
      std::ostringstream oss;
      oss << "DELETE FROM index_data WHERE index_id=" << i;
      std::ostringstream oss_error;
      oss_error << "storage: cannot delete index " << i << ": ";
      _mysql.run_query(oss.str(), oss_error.str(), false, conn);
      _add_action(conn, actions::index_data);

      // Remove associated graph.
      std::shared_ptr<storage::remove_graph>
        rg{std::make_shared<storage::remove_graph>(index_id, true)};
      multiplexing::publisher().write(rg);
    }

  }

  // End.
  logging::info(logging::medium)
    << "storage: end of DB cleanup: " << deleted_metrics << " metrics and "
    << deleted_index << " index removed";
  // _update_status("");
}
