/*
** Copyright 2012-2015,2017,2020 Centreon
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

#include "com/centreon/broker/storage/rebuilder.hh"

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_cfg                  Database configuration.
 *  @param[in] rebuild_check_interval  How often the rebuild thread will
 *                                     check for rebuild.
 *  @param[in] rrd_length              Length of RRD files.
 *  @param[in] interval_length         Length in seconds of a time unit.
 */
rebuilder::rebuilder(database_config const& db_cfg,
                     uint32_t rebuild_check_interval,
                     uint32_t rrd_length,
                     uint32_t interval_length)
    : _db_cfg(db_cfg),
      _interval_length(interval_length),
      _rebuild_check_interval(rebuild_check_interval),
      _rrd_len(rrd_length),
      _should_exit(false) {
  _db_cfg.set_connections_count(1);
  _db_cfg.set_queries_per_transaction(1);
  _thread.reset(new std::thread(&rebuilder::_run, this));
}

/**
 *  Destructor.
 */
rebuilder::~rebuilder() {
  std::unique_lock<std::mutex> lock(_mutex_should_exit);
  _should_exit = true;
  lock.unlock();
  _cond_should_exit.notify_all();
  _thread->join();
}

/**
 *  Get the rebuild check interval.
 *
 *  @return Rebuild check interval in seconds.
 */
uint32_t rebuilder::get_rebuild_check_interval() const noexcept {
  return _rebuild_check_interval;
}

/**
 *  Get the RRD length in seconds.
 *
 *  @return RRD length in seconds.
 */
uint32_t rebuilder::get_rrd_length() const noexcept {
  return _rrd_len;
}

/**
 *  Thread entry point.
 */
void rebuilder::_run() {
  std::unique_lock<std::mutex> locker(_mutex_should_exit);
  while (!_should_exit && _rebuild_check_interval) {
    try {
      // Open DB.
      std::unique_ptr<mysql> ms;
      try {
        ms.reset(new mysql(_db_cfg));
      } catch (std::exception const& e) {
        throw broker::exceptions::msg()
            << "storage: rebuilder: could "
               "not connect to Centreon Storage database: "
            << e.what();
      }

      // Fetch index to rebuild.
      index_info info;
      _next_index_to_rebuild(info, *ms);
      while (!_should_exit && info.index_id) {
        // Get check interval of host/service.
        uint32_t index_id;
        uint32_t host_id;
        uint32_t service_id;
        uint32_t check_interval(0);
        uint32_t rrd_len;
        {
          index_id = info.index_id;
          host_id = info.host_id;
          service_id = info.service_id;
          rrd_len = info.rrd_retention;

          std::ostringstream oss;
          if (!info.service_id)
            oss << "SELECT check_interval FROM hosts"
                   " WHERE host_id="
                << info.host_id;
          else
            oss << "SELECT check_interval FROM services WHERE host_id="
                << info.host_id << " AND service_id=" << info.service_id;
          std::promise<database::mysql_result> promise;
          ms->run_query_and_get_result(oss.str(), &promise);
          database::mysql_result res(promise.get_future().get());
          if (ms->fetch_row(res))
            check_interval = res.value_as_f64(0) * _interval_length;
          if (!check_interval)
            check_interval = 5 * 60;
        }
        log_v2::sql()->info(
            "storage: rebuilder: index {} (interval {}) will be rebuild",
            index_id, check_interval);

        // Set index as being rebuilt.
        _set_index_rebuild(*ms, index_id, 2);

        try {
          // Fetch metrics to rebuild.
          std::list<metric_info> metrics_to_rebuild;
          {
            std::ostringstream oss;
            oss << "SELECT metric_id, metric_name, data_source_type"
                   " FROM metrics WHERE index_id="
                << index_id;

            std::promise<database::mysql_result> promise;
            ms->run_query_and_get_result(oss.str(), &promise);
            try {
              database::mysql_result res(promise.get_future().get());

              while (!_should_exit && ms->fetch_row(res)) {
                metric_info info;
                info.metric_id = res.value_as_u32(0);
                info.metric_name = res.value_as_str(1);
                info.metric_type = res.value_as_str(2)[0] - '0';
                metrics_to_rebuild.push_back(info);
              }
            } catch (std::exception const& e) {
              throw exceptions::msg()
                  << "storage: rebuilder: could not fetch metrics of index "
                  << index_id << ": " << e.what();
            }
          }

          // Browse metrics to rebuild.
          while (!_should_exit && !metrics_to_rebuild.empty()) {
            metric_info& info(metrics_to_rebuild.front());
            _rebuild_metric(*ms, info.metric_id, host_id, service_id,
                            info.metric_name, info.metric_type, check_interval,
                            rrd_len);
            // We need to update the conflict_manager for metrics that could
            // change of type.
            conflict_manager::instance().update_metric_info_cache(
                index_id, info.metric_id, info.metric_name, info.metric_type);
            metrics_to_rebuild.pop_front();
          }

          // Rebuild status.
          _rebuild_status(*ms, index_id, check_interval, rrd_len);
        } catch (...) {
          // Set index as to-be-rebuilt.
          _set_index_rebuild(*ms, index_id, 1);

          // Rethrow exception.
          throw;
        }

        // Set index as rebuilt or to-be-rebuild
        // if we were interrupted.
        _set_index_rebuild(*ms, index_id, (_should_exit ? 1 : 0));

        // Get next index to rebuild.
        _next_index_to_rebuild(info, *ms);
      }
    } catch (std::exception const& e) {
      logging::error(logging::high) << e.what();
    } catch (...) {
      logging::error(logging::high) << "storage: rebuilder: unknown error";
    }

    // Sleep a while.
    _cond_should_exit.wait_for(locker,
                               std::chrono::seconds(_rebuild_check_interval));
  }
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Get next index to rebuild.
 *
 *  @param[out] info  Information about the next index to rebuild.
 *                    Zero'd if no index is waiting for rebuild.
 *  @param[in]  db    Database object.
 */
void rebuilder::_next_index_to_rebuild(index_info& info, mysql& ms) {
  const std::string query(
      "SELECT id,host_id,service_id,rrd_retention FROM"
      " index_data WHERE must_be_rebuild='1' LIMIT 1");
  std::promise<database::mysql_result> promise;
  ms.run_query_and_get_result(query, &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    if (ms.fetch_row(res)) {
      info.index_id = res.value_as_u32(0);
      info.host_id = res.value_as_u32(1);
      info.service_id = res.value_as_u32(2);
      info.rrd_retention = res.value_as_u32(3);
      if (!info.rrd_retention)
        info.rrd_retention = _rrd_len;
    } else
      memset(&info, 0, sizeof(info));
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "storage: rebuilder: could not fetch index to rebuild: " << e.what();
  }
}

/**
 *  Rebuild a metric.
 *
 *  @param[in] db           Database object.
 *  @param[in] metric_id    Metric ID.
 *  @param[in] host_id      Id of the host this metric belong to.
 *  @param[in] service_id   Id of the service this metric belong to.
 *  @param[in] metric_name  Metric name.
 *  @param[in] type         Metric type.
 *  @param[in] interval     Host/service check interval.
 *  @param[in] length       Metric RRD length in seconds.
 */
void rebuilder::_rebuild_metric(mysql& ms,
                                uint32_t metric_id,
                                uint32_t host_id,
                                uint32_t service_id,
                                std::string const& metric_name,
                                short metric_type,
                                uint32_t interval,
                                uint32_t length) {
  // Log.
  log_v2::sql()->info(
      "storage: rebuilder: rebuilding metric {} (name {}, type {}, interval "
      "{})",
      metric_id, metric_name, metric_type, interval);

  // Send rebuild start event.
  _send_rebuild_event(false, metric_id, false);

  time_t start(time(nullptr) - length);

  try {
    // Get data.
    std::ostringstream oss;
    oss << "SELECT ctime, value FROM data_bin WHERE id_metric=" << metric_id
        << " AND ctime>=" << start << " ORDER BY ctime ASC";
    std::promise<database::mysql_result> promise;
    ms.run_query_and_get_result(oss.str(), &promise);
    log_v2::sql()->debug(
        "storage(rebuilder): rebuild of metric {}: SQL query: \"{}\"",
        metric_id, oss.str());

    try {
      database::mysql_result res(promise.get_future().get());
      while (!_should_exit && ms.fetch_row(res)) {
        std::shared_ptr<storage::metric> entry =
            std::make_shared<storage::metric>(
                host_id, service_id, metric_name, res.value_as_u32(0), interval,
                true, metric_id, length, res.value_as_f64(1), metric_type);
        if (entry->value > FLT_MAX * 0.999)
          entry->value = INFINITY;
        else if (entry->value < -FLT_MAX * 0.999)
          entry->value = -INFINITY;
        log_v2::perfdata()->trace(
            "storage(rebuilder): Sending metric with host_id {}, service_id "
            "{}, metric_name {}, ctime {}, interval {}, is_for_rebuild {}, "
            "metric_id {}, rrd_len {}, value {}, value_type{}",
            host_id, service_id, metric_name, res.value_as_u32(0), interval,
            true, metric_id, length, res.value_as_f64(1), metric_type);

        multiplexing::publisher().write(entry);
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "storage: rebuilder: "
          << "cannot fetch data of metric " << metric_id << ": " << e.what();
    }
  } catch (...) {
    // Send rebuild end event.
    _send_rebuild_event(true, metric_id, false);

    // Rethrow exception.
    throw;
  }

  // Send rebuild end event.
  _send_rebuild_event(true, metric_id, false);
}

/**
 *  Rebuild a status.
 *
 *  @param[in] db        Database object.
 *  @param[in] index_id  Index ID.
 *  @param[in] interval  Host/service check interval.
 */
void rebuilder::_rebuild_status(mysql& ms,
                                uint32_t index_id,
                                uint32_t interval,
                                uint32_t length) {
  // Log.
  log_v2::sql()->info("storage: rebuilder: rebuilding status {} (interval {})",
                      index_id, interval);

  // Send rebuild start event.
  _send_rebuild_event(false, index_id, true);

  time_t start(time(nullptr) - length);

  // Database schema version.
  try {
    // Get data.
    std::ostringstream oss;
    oss << "SELECT d.ctime, d.status FROM metrics AS m JOIN data_bin AS d"
           " ON m.metric_id=d.id_metric WHERE m.index_id="
        << index_id << " AND ctime>=" << start << " ORDER BY d.ctime ASC";
    std::promise<database::mysql_result> promise;
    ms.run_query_and_get_result(oss.str(), &promise);
    try {
      database::mysql_result res(promise.get_future().get());
      while (!_should_exit && ms.fetch_row(res)) {
        std::shared_ptr<storage::status> entry(
            std::make_shared<storage::status>(res.value_as_u32(0), index_id,
                                              interval, true, _rrd_len,
                                              res.value_as_i32(1)));
        multiplexing::publisher().write(entry);
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "storage: rebuilder: "
          << "cannot fetch data of index " << index_id << ": " << e.what();
    }
  } catch (...) {
    // Send rebuild end event.
    _send_rebuild_event(true, index_id, true);

    // Rethrow exception.
    throw;
  }

  // Send rebuild end event.
  _send_rebuild_event(true, index_id, true);
}

/**
 *  Send a rebuild event.
 *
 *  @param[in] end      false if rebuild is starting, true if it is ending.
 *  @param[in] id       Index or metric ID.
 *  @param[in] is_index true for an index ID, false for a metric ID.
 */
void rebuilder::_send_rebuild_event(bool end, uint32_t id, bool is_index) {
  std::shared_ptr<storage::rebuild> rb =
      std::make_shared<storage::rebuild>(end, id, is_index);
  multiplexing::publisher().write(rb);
}

/**
 *  Set index rebuild flag.
 *
 *  @param[in] db        Database object.
 *  @param[in] index_id  Index to update.
 *  @param[in] state     Rebuild state (0, 1 or 2).
 */
void rebuilder::_set_index_rebuild(mysql& ms, uint32_t index_id, short state) {
  std::string query(fmt::format("UPDATE index_data SET must_be_rebuild='{}' WHERE id={}", state, index_id));
  std::string err(fmt::format("storage: rebuilder: cannot update state of index {}: ", index_id));
  ms.run_query(query, err, false);
}
