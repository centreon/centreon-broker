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
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

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
 */
void conflict_manager::_storage_process_service_status() {
  logging::debug(logging::low) << "storage: process_service_status...";

  auto& p = _events.front();
  std::shared_ptr<io::data> d{std::get<0>(p)};
  neb::service_status const& ss{*static_cast<neb::service_status*>(d.get())};
  uint64_t host_id = ss.host_id, service_id = ss.service_id;
  auto it = _index_cache.find({host_id, service_id});
  uint32_t index_id, rrd_len;
  int32_t conn =
      _mysql.choose_connection_by_instance(_cache_host_instance[ss.host_id]);

  /* Index does not exist */
  if (it == _index_cache.end()) {
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
    _index_data_insert.bind_value_as_str(5,
                                         "0");  // FIXME DBR is given by special
    std::promise<int> promise;
    _mysql.run_statement_and_get_int(
        _index_data_insert, &promise, database::mysql_task::LAST_INSERT_ID, conn);
    try {
      index_id = promise.get_future().get();
      if (index_id == 0) {
        throw broker::exceptions::msg()
            << "storage: could not fetch index_id of newly inserted index ("
            << host_id << ", " << service_id << ")";
      }

      /* Insert index in cache. */
      logging::info(logging::medium) << "storage: new index " << index_id
                                     << " for (" << host_id << ", "
                                     << service_id << ")";
      index_info info{
          .host_name = ss.host_name,
          .index_id = index_id,
          .locked = false,
          .rrd_retention = _rrd_len,
          .service_description = ss.service_description,
          .special = false  // FIXME DBR: is also configurable
      };
      _index_cache[{host_id, service_id}] = std::move(info);
      rrd_len = _rrd_len;

      /* Create the metric mapping. */
      std::shared_ptr<storage::index_mapping> im{
          std::make_shared<storage::index_mapping>(
              index_id, host_id, service_id)};
      multiplexing::publisher pblshr;
      pblshr.write(im);
    }
    catch (std::exception const& e) {
      throw broker::exceptions::msg() << "storage: insertion of index ("
                                      << host_id << ", " << service_id
                                      << ") failed: " << e.what();
    }
  }
  else {
    index_id = it->second.index_id;
    rrd_len = it->second.rrd_retention;
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
      /* Parse perfdata. */
      std::list<storage::perfdata> pds;
      storage::parser p;
      try {
        p.parse_perfdata(ss.perf_data, pds);

        for (storage::perfdata& pd : pds) {
          auto it = _metric_cache.find({index_id, pd.name()});

          /* The cache does not contain this metric */
          uint32_t metric_id;
          if (it == _metric_cache.end()) {
            /* Let's insert it */
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
            metric_id = it->second.metric_id;

            logging::debug(logging::low) << "storage: found metric "
                                         << it->second.metric_id << " of ("
                                         << index_id << ", " << pd.name()
                                         << ") in cache";
            // Should we update metrics ?
            if (!check_equality(it->second.value, pd.value()) ||
                it->second.unit_name != pd.unit() ||
                !check_equality(it->second.warn, pd.warning()) ||
                !check_equality(it->second.warn_low, pd.warning_low()) ||
                it->second.warn_mode != pd.warning_mode() ||
                !check_equality(it->second.crit, pd.critical()) ||
                !check_equality(it->second.crit_low, pd.critical_low()) ||
                it->second.crit_mode != pd.critical_mode() ||
                !check_equality(it->second.min, pd.min()) ||
                !check_equality(it->second.max, pd.max())) {
              logging::info(logging::medium)
                  << "storage: updating metric " << it->second.metric_id
                  << " of (" << index_id << ", " << pd.name()
                  << ") (unit: " << pd.unit()
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
              _metrics_update.bind_value_as_i32(10, it->second.metric_id);

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
        }
      }
      catch (storage::exceptions::perfdata const& e) {
        logging::error(logging::medium)
          << "storage: error while parsing perfdata of service ("
          << host_id << ", " << service_id << "): " << e.what();
      }
    }
  }

  *std::get<2>(p) = true;
  _events.pop_front();
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
    logging::info(logging::low) << "storage: " << count << " perfdatas inserted in data_bin";
  }
}
