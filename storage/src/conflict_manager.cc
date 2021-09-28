/*
** Copyright 2019-2020 Centreon
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
#include "com/centreon/broker/storage/conflict_manager.hh"

#include <cassert>
#include <cstring>

#include "com/centreon/broker/database/mysql_result.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/perfdata.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::database;
using namespace com::centreon::broker::storage;

conflict_manager* conflict_manager::_singleton = nullptr;
conflict_manager::instance_state conflict_manager::_state{
    conflict_manager::not_started};
std::mutex conflict_manager::_init_m;
std::condition_variable conflict_manager::_init_cv;

void (conflict_manager::*const conflict_manager::_neb_processing_table[])(
    std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>&) = {
    nullptr,
    &conflict_manager::_process_acknowledgement,
    &conflict_manager::_process_comment,
    &conflict_manager::_process_custom_variable,
    &conflict_manager::_process_custom_variable_status,
    &conflict_manager::_process_downtime,
    &conflict_manager::_process_event_handler,
    &conflict_manager::_process_flapping_status,
    &conflict_manager::_process_host_check,
    &conflict_manager::_process_host_dependency,
    &conflict_manager::_process_host_group,
    &conflict_manager::_process_host_group_member,
    &conflict_manager::_process_host,
    &conflict_manager::_process_host_parent,
    &conflict_manager::_process_host_status,
    &conflict_manager::_process_instance,
    &conflict_manager::_process_instance_status,
    &conflict_manager::_process_log,
    &conflict_manager::_process_module,
    &conflict_manager::_process_service_check,
    &conflict_manager::_process_service_dependency,
    &conflict_manager::_process_service_group,
    &conflict_manager::_process_service_group_member,
    &conflict_manager::_process_service,
    &conflict_manager::_process_service_status,
    &conflict_manager::_process_instance_configuration,
    &conflict_manager::_process_responsive_instance,
};

conflict_manager& conflict_manager::instance() {
  assert(_singleton);
  return *_singleton;
}

conflict_manager::conflict_manager(database_config const& dbcfg,
                                   uint32_t loop_timeout,
                                   uint32_t instance_timeout)
    : _exit{false},
      _broken{false},
      _loop_timeout{loop_timeout},
      _max_pending_queries(dbcfg.get_queries_per_transaction()),
      _mysql{dbcfg},
      _instance_timeout{instance_timeout},
      _store_in_db{true},
      _rrd_len{0},
      _interval_length{0},
      _max_perfdata_queries{0},
      _max_metrics_queries{0},
      _max_cv_queries{0},
      _stats{stats::center::instance().register_conflict_manager()},
      _max_log_queries{0},
      _events_handled{0},
      _speed{},
      _stats_count_pos{0},
      _ref_count{0},
      _oldest_timestamp{std::numeric_limits<time_t>::max()} {
  log_v2::sql()->debug("conflict_manager: class instanciation");
  stats::center::instance().update(&ConflictManagerStats::set_loop_timeout,
                                   _stats, _loop_timeout);
  stats::center::instance().update(
      &ConflictManagerStats::set_max_pending_events, _stats,
      _max_pending_queries);
}

conflict_manager::~conflict_manager() {
  log_v2::sql()->debug("conflict_manager: destruction");
}

/**
 * For the connector that does not initialize the conflict_manager, this
 * function is useful to wait.
 *
 * @param store_in_db A boolean to specify if perfdata should be stored in
 * database.
 * @param rrd_len The rrd length in seconds
 * @param interval_length The length of an elementary time interval.
 * @param queries_per_transaction The number of perfdata to store before sending
 * them to database.
 *
 * @return true if all went OK.
 */
bool conflict_manager::init_storage(bool store_in_db,
                                    uint32_t rrd_len,
                                    uint32_t interval_length,
                                    uint32_t queries_per_transaction) {
  log_v2::sql()->debug("conflict_manager: storage stream initialization");
  int count;

  std::unique_lock<std::mutex> lk(_init_m);

  for (count = 0; count < 10; count++) {
    /* Let's wait for 10s for the conflict_manager to be initialized */
    if (_init_cv.wait_for(lk, std::chrono::seconds(1), [&] {
          return _singleton != nullptr || _state == finished;
        })) {
      if (_state == finished)
        return false;
      std::lock_guard<std::mutex> lk(_singleton->_loop_m);
      _singleton->_store_in_db = store_in_db;
      _singleton->_rrd_len = rrd_len;
      _singleton->_interval_length = interval_length;
      _singleton->_max_perfdata_queries = queries_per_transaction;
      _singleton->_max_metrics_queries = queries_per_transaction;
      _singleton->_max_cv_queries = queries_per_transaction;
      _singleton->_max_log_queries = queries_per_transaction;
      _singleton->_ref_count++;
      _singleton->_thread =
          std::move(std::thread(&conflict_manager::_callback, _singleton));
      pthread_setname_np(_singleton->_thread.native_handle(), "conflict_mngr");
      return true;
    }
    log_v2::sql()->info(
        "conflict_manager: Waiting for the sql stream initialization for {} "
        "seconds",
        count);
  }
  log_v2::sql()->error(
      "conflict_manager: not initialized after 10s. Probably "
      "an issue in the sql output configuration.");
  return false;
}

/**
 * @brief This fonction is the one that initializes the conflict_manager.
 *
 * @param dbcfg The database configuration
 * @param loop_timeout A duration in seconds. During this interval received
 *        events are handled. If there are no more events to handle, new
 *        available ones are taken from the fifo. If none, the loop waits during
 *        500ms. After this loop others things are done, cleanups, etc. And then
 *        the loop is started again.
 * @param instance_timeout A duration in seconds. This interval is used for
 *        sending data in bulk. We wait for this interval at least between two
 *        bulks.
 *
 * @return A boolean true if the function went good, false otherwise.
 */
bool conflict_manager::init_sql(database_config const& dbcfg,
                                uint32_t loop_timeout,
                                uint32_t instance_timeout) {
  log_v2::sql()->debug("conflict_manager: sql stream initialization");
  std::lock_guard<std::mutex> lk(_init_m);
  _singleton = new conflict_manager(dbcfg, loop_timeout, instance_timeout);
  if (!_singleton) {
    _state = finished;
    return false;
  }

  _state = running;
  _singleton->_action.resize(_singleton->_mysql.connections_count());
  _init_cv.notify_all();
  _singleton->_ref_count++;
  return true;
}

void conflict_manager::_load_deleted_instances() {
  _cache_deleted_instance_id.clear();
  std::string query{"SELECT instance_id FROM instances WHERE deleted=1"};
  std::promise<mysql_result> promise;
  _mysql.run_query_and_get_result(query, &promise);
  try {
    mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res))
      _cache_deleted_instance_id.insert(res.value_as_u32(0));
  } catch (std::exception const& e) {
    throw msg_fmt("could not get list of deleted instances: {}", e.what());
  }
}

void conflict_manager::_load_caches() {
  // Fill index cache.
  std::lock_guard<std::mutex> lk(_loop_m);

  /* get deleted cache of instance ids => _cache_deleted_instance_id */
  _load_deleted_instances();

  /* get all outdated instances from the database => _stored_timestamps */
  {
    std::string query{"SELECT instance_id FROM instances WHERE outdated=TRUE"};
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        uint32_t instance_id = res.value_as_i32(0);
        _stored_timestamps.insert(
            {instance_id,
             stored_timestamp(instance_id, stored_timestamp::unresponsive)});
        stored_timestamp& ts = _stored_timestamps[instance_id];
        ts.set_timestamp(timestamp(std::numeric_limits<time_t>::max()));
      }
    } catch (std::exception const& e) {
      throw msg_fmt(
          "conflict_manager: could not get the list of outdated instances: {}",
          e.what());
    }
  }

  /* index_data => _index_cache */
  {
    // Execute query.
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT "
        "id,host_id,service_id,host_name,rrd_retention,service_description,"
        "special,locked FROM index_data",
        &promise);
    try {
      database::mysql_result res(promise.get_future().get());

      // Loop through result set.
      while (_mysql.fetch_row(res)) {
        index_info info{.host_name = res.value_as_str(3),
                        .index_id = res.value_as_u64(0),
                        .locked = res.value_as_bool(7),
                        .rrd_retention = res.value_as_u32(4)
                                             ? res.value_as_u32(4)
                                             : _rrd_len,
                        .service_description = res.value_as_str(5),
                        .special = res.value_as_u32(6) == 2};
        uint32_t host_id(res.value_as_u32(1));
        uint32_t service_id(res.value_as_u32(2));
        log_v2::perfdata()->debug(
            "storage: loaded index {} of ({}, {}) with rrd_len={}",
            info.index_id, host_id, service_id, info.rrd_retention);
        _index_cache[{host_id, service_id}] = std::move(info);

        // Create the metric mapping.
        std::shared_ptr<storage::index_mapping> im{
            std::make_shared<storage::index_mapping>(info.index_id, host_id,
                                                     service_id)};
        multiplexing::publisher pblshr;
        pblshr.write(im);
      }
    } catch (std::exception const& e) {
      throw msg_fmt("storage: could not fetch index list from data DB: {}",
                    e.what());
    }
  }

  /* hosts => _cache_host_instance */
  {
    _cache_host_instance.clear();

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result("SELECT host_id,instance_id FROM hosts",
                                    &promise);

    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        _cache_host_instance[res.value_as_u32(0)] = res.value_as_u32(1);
    } catch (std::exception const& e) {
      throw msg_fmt("SQL: could not get the list of host/instance pairs: {}",
                    e.what());
    }
  }

  /* hostgroups => _hostgroup_cache */
  {
    _hostgroup_cache.clear();

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result("SELECT hostgroup_id FROM hostgroups",
                                    &promise);

    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        _hostgroup_cache.insert(res.value_as_u32(0));
    } catch (std::exception const& e) {
      throw msg_fmt("SQL: could not get the list of hostgroups id: {}",
                    e.what());
    }
  }

  /* servicegroups => _servicegroup_cache */
  {
    _servicegroup_cache.clear();

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result("SELECT servicegroup_id FROM servicegroups",
                                    &promise);

    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        _servicegroup_cache.insert(res.value_as_u32(0));
    } catch (std::exception const& e) {
      throw msg_fmt("SQL: could not get the list of servicegroups id: {}",
                    e.what());
    }
  }

  _cache_svc_cmd.clear();
  _cache_hst_cmd.clear();

  /* metrics => _metric_cache */
  {
    std::lock_guard<std::mutex> lock(_metric_cache_m);
    _metric_cache.clear();
    _metrics.clear();

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT "
        "metric_id,index_id,metric_name,unit_name,warn,warn_low,"
        "warn_threshold_mode,crit,crit_low,crit_threshold_mode,min,max,"
        "current_value,data_source_type FROM metrics",
        &promise);

    try {
      mysql_result res{promise.get_future().get()};
      while (_mysql.fetch_row(res)) {
        metric_info info;
        info.metric_id = res.value_as_u32(0);
        info.locked = false;
        info.unit_name = res.value_as_str(3);
        info.warn = res.value_as_f32(4);
        info.warn_low = res.value_as_f32(5);
        info.warn_mode = res.value_as_i32(6);
        info.crit = res.value_as_f32(7);
        info.crit_low = res.value_as_f32(8);
        info.crit_mode = res.value_as_i32(9);
        info.min = res.value_as_f32(10);
        info.max = res.value_as_f32(11);
        info.value = res.value_as_f32(12);
        info.type = res.value_as_str(13)[0] - '0';
        info.metric_mapping_sent = false;
        _metric_cache[{res.value_as_u64(1), res.value_as_str(2)}] = info;
      }
    } catch (std::exception const& e) {
      throw msg_fmt("conflict_manager: could not get the list of metrics: {}",
                    e.what());
    }
  }
}

void conflict_manager::update_metric_info_cache(uint64_t index_id,
                                                uint32_t metric_id,
                                                std::string const& metric_name,
                                                short metric_type) {
  auto it = _metric_cache.find({index_id, metric_name});
  if (it != _metric_cache.end()) {
    log_v2::perfdata()->info(
        "conflict_manager: updating metric '{}' of id {} at index {} to "
        "metric_type {}",
        metric_name, metric_id, index_id,
        perfdata::data_type_name[metric_type]);
    std::lock_guard<std::mutex> lock(_metric_cache_m);
    it->second.type = metric_type;
    if (it->second.metric_id != metric_id) {
      it->second.metric_id = metric_id;
      // We need to repopulate a new metric_mapping
      it->second.metric_mapping_sent = false;
    }
  }
}

/**
 *  The main loop of the conflict_manager
 */
void conflict_manager::_callback() {
  try {
    _load_caches();
  } catch (std::exception const& e) {
    log_v2::sql()->error("error while loading caches: {}", e.what());
    _broken = true;
  }

  do {
    std::chrono::system_clock::time_point time_to_deleted_index =
        std::chrono::system_clock::now();

    size_t pos = 0;
    std::deque<std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>> events;
    try {
      while (!_should_exit()) {
        /* Time to send perfdatas to rrd ; no lock needed, it is this thread
         * that fill this queue. */
        _insert_perfdatas();

        /* Time to send metrics to database */
        _update_metrics();

        /* Time to send customvariables to database */
        _update_customvariables();

        /* Time to send logs to database */
        _insert_logs();

        log_v2::sql()->trace(
            "conflict_manager: main loop initialized with a timeout of {} "
            "seconds.",
            _loop_timeout);

        std::chrono::system_clock::time_point now0 =
            std::chrono::system_clock::now();

        /* Are there index_data to remove? */
        if (now0 >= time_to_deleted_index) {
          try {
            _check_deleted_index();
            time_to_deleted_index += std::chrono::minutes(5);
          } catch (std::exception const& e) {
            log_v2::sql()->error(
                "conflict_manager: error while checking deleted indexes: {}",
                e.what());
            _broken = true;
            break;
          }
        }
        int32_t count = 0;
        int32_t timeout = 0;
        int32_t timeout_limit = _loop_timeout * 1000;

        /* This variable is incremented 1000 by 1000 and represents
         * milliseconds. Each time the duration reaches this value, we make
         * stuffs. We make then a timer cadenced at 1000ms. */
        int32_t duration = 1000;

        time_t next_insert_perfdatas = time(nullptr);
        time_t next_update_metrics = next_insert_perfdatas;
        time_t next_update_cv = next_insert_perfdatas;
        time_t next_update_log = next_insert_perfdatas;

        auto empty_caches = [this, &next_insert_perfdatas, &next_update_metrics,
                             &next_update_cv, &next_update_log](
                                std::chrono::system_clock::time_point now) {
          /* If there are too many perfdata to send, let's send them... */
          if (std::chrono::system_clock::to_time_t(now) >=
                  next_insert_perfdatas ||
              _perfdata_queue.size() > _max_perfdata_queries) {
            next_insert_perfdatas =
                std::chrono::system_clock::to_time_t(now) + 10;
            _insert_perfdatas();
          }

          /* If there are too many metrics to send, let's send them... */
          if (std::chrono::system_clock::to_time_t(now) >=
                  next_update_metrics ||
              _metrics.size() > _max_metrics_queries) {
            next_update_metrics =
                std::chrono::system_clock::to_time_t(now) + 10;
            _update_metrics();
          }

          /* Time to send customvariables to database */
          if (std::chrono::system_clock::to_time_t(now) >= next_update_cv ||
              _cv_queue.size() + _cvs_queue.size() > _max_cv_queries) {
            next_update_cv = std::chrono::system_clock::to_time_t(now) + 10;
            _update_customvariables();
          }

          /* Time to send logs to database */
          if (std::chrono::system_clock::to_time_t(now) >= next_update_log ||
              _log_queue.size() > _max_log_queries) {
            next_update_log = std::chrono::system_clock::to_time_t(now) + 10;
            _insert_logs();
          }
        };

        /* During this loop, connectors still fill the queue when they receive
         * new events.
         * The loop is hold by three conditions that are:
         * - events.empty() no more events to treat.
         * - count < _max_pending_queries: we don't want to commit everytimes,
         *   so we keep this count to know if we reached the
         *   _max_pending_queries parameter.
         * - timeout < timeout_limit: If the loop lives too long, we interrupt
         * it it is necessary for cleanup operations.
         */
        while (count < _max_pending_queries && timeout < timeout_limit) {
          if (events.empty())
            events = _fifo.first_events();
          if (events.empty()) {
            // Let's wait for 500ms.
            if (_should_exit())
              break;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            /* Here, just before looping, we commit. */
            std::chrono::system_clock::time_point now =
                std::chrono::system_clock::now();

            empty_caches(now);
            _finish_actions();
            continue;
          }
          while (!events.empty()) {
            auto tpl = events.front();
            events.pop_front();
            std::shared_ptr<io::data>& d = std::get<0>(tpl);
            uint32_t type{d->type()};
            uint16_t cat{io::events::category_of_type(type)};
            uint16_t elem{io::events::element_of_type(type)};
            if (std::get<1>(tpl) == sql && cat == io::events::neb)
              (this->*(_neb_processing_table[elem]))(tpl);
            else if (std::get<1>(tpl) == storage && cat == io::events::neb &&
                     type == neb::service_status::static_type())
              _storage_process_service_status(tpl);
            else {
              log_v2::sql()->trace(
                  "conflict_manager: event of type {} thrown away ; no need to "
                  "store it in the database.",
                  type);
              *std::get<2>(tpl) = true;
            }

            ++count;
            _stats_count[pos]++;

            std::chrono::system_clock::time_point now1 =
                std::chrono::system_clock::now();

            empty_caches(now1);

            timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now1 - now0)
                          .count();

            /* Get some stats each second */
            if (timeout >= duration) {
              do {
                duration += 1000;
                pos++;
                if (pos >= _stats_count.size())
                  pos = 0;
                _stats_count[pos] = 0;
              } while (timeout > duration);

              _events_handled = events.size();
              float s = 0.0f;
              for (const auto& c : _stats_count)
                s += c;

              std::lock_guard<std::mutex> lk(_stat_m);
              _speed = s / _stats_count.size();
              stats::center::instance().update(&ConflictManagerStats::set_speed,
                                               _stats,
                                               static_cast<double>(_speed));
            }
          }
        }
        log_v2::sql()->debug("{} new events to treat", count);
        /* Here, just before looping, we commit. */
        _finish_actions();
        if (_fifo.get_pending_elements() == 0)
          log_v2::sql()->debug(
              "conflict_manager: acknowledgement - no pending events");
        else
          log_v2::sql()->debug(
              "conflict_manager: acknowledgement - still {} not acknowledged",
              _fifo.get_pending_elements());

        /* Are there unresonsive instances? */
        _update_hosts_and_services_of_unresponsive_instances();

        /* Get some stats */
        {
          std::lock_guard<std::mutex> lk(_stat_m);
          _events_handled = events.size();
          stats::center::instance().update(
              &ConflictManagerStats::set_events_handled, _stats,
              _events_handled);
          stats::center::instance().update(
              &ConflictManagerStats::set_max_perfdata_events, _stats,
              _max_perfdata_queries);
          stats::center::instance().update(
              &ConflictManagerStats::set_waiting_events, _stats,
              static_cast<int32_t>(_fifo.get_events().size()));
          stats::center::instance().update(
              &ConflictManagerStats::set_sql, _stats,
              static_cast<int32_t>(_fifo.get_timeline(sql).size()));
          stats::center::instance().update(
              &ConflictManagerStats::set_storage, _stats,
              static_cast<int32_t>(_fifo.get_timeline(storage).size()));
        }
      }
    } catch (std::exception const& e) {
      log_v2::sql()->error("conflict_manager: error in the main loop: {}",
                           e.what());
      if (strstr(e.what(), "server has gone away")) {
        // The case where we must restart the connector.
        _broken = true;
      }
    }
  } while (!_should_exit());

  if (_broken) {
    std::unique_lock<std::mutex> lk(_loop_m);
    /* Let's wait for the end */
    log_v2::sql()->info(
        "conflict_manager: waiting for the end of the conflict manager main "
        "loop.");
    _loop_cv.wait(lk, [this]() { return !_exit; });
  }
}

/**
 *  Tell if the main loop can exit. Two conditions are needed:
 *    * _exit = true
 *    * _events is empty.
 *
 * This methods takes the lock on _loop_m, so don't call it if you already have
 * it.
 *
 * @return True if the loop can be interrupted, false otherwise.
 */
bool conflict_manager::_should_exit() const {
  std::lock_guard<std::mutex> lock(_loop_m);
  return _broken || (_exit && _fifo.get_events().empty());
}

/**
 *  Method to send event to the conflict manager.
 *
 * @param c The connector responsible of the event (sql or storage)
 * @param e The event
 *
 * @return The number of events to ack.
 */
int32_t conflict_manager::send_event(conflict_manager::stream_type c,
                                     std::shared_ptr<io::data> const& e) {
  assert(e);
  if (_broken)
    throw msg_fmt("conflict_manager: events loop interrupted");

  log_v2::sql()->trace(
      "conflict_manager: send_event category:{}, element:{} from {}",
      e->type() >> 16, e->type() & 0xffff, c == 0 ? "sql" : "storage");

  return _fifo.push(c, e);
}

/**
 *  This method is called from the stream and returns how many events should
 *  be released. By the way, it removes those objects from the queue.
 *
 * @param c a stream_type (we have two kinds of data arriving in the
 * conflict_manager, those from the sql connector and those from the storage
 * connector, so this stream_type is an enum containing those types).
 *
 * @return the number of events to ack.
 */
int32_t conflict_manager::get_acks(stream_type c) {
  if (_broken)
    throw msg_fmt("conflict_manager: events loop interrupted");

  return _fifo.get_acks(c);
}

/**
 *  Take a look if a given action is done on a mysql connection. If it is
 *  done, the method waits for tasks on this connection to be finished and
 *  clear the flag.
 *  In case of a conn < 0, the methods checks all the connections.
 *
 * @param conn The connection number or a negative number to check all the
 *             connections
 * @param action An action.
 */
void conflict_manager::_finish_action(int32_t conn, uint32_t action) {
  if (conn < 0) {
    for (std::size_t i = 0; i < _action.size(); i++) {
      if (_action[i] & action) {
        _mysql.commit(i);
        _action[i] = actions::none;
      }
    }
  } else if (_action[conn] & action) {
    _mysql.commit(conn);
    _action[conn] = actions::none;
  }
}

/**
 *  The main goal of this method is to commit queries sent to the db.
 *  When the commit is done (all the connections commit), we count how
 *  many events can be acknowledged. So we can also update the number of pending
 *  events.
 */
void conflict_manager::_finish_actions() {
  log_v2::sql()->trace("conflict_manager: finish actions");
  _mysql.commit();
  for (uint32_t& v : _action)
    v = actions::none;

  _fifo.clean(sql);
  _fifo.clean(storage);

  log_v2::sql()->debug("conflict_manager: still {} not acknowledged",
                       _fifo.get_pending_elements());
}

/**
 *  Add an action on the connection conn in the list of current actions.
 *  If conn < 0, the action is added to all the connections.
 *
 * @param conn The connection number or a negative number to add to all the
 *             connections
 * @param action An action.
 */
void conflict_manager::_add_action(int32_t conn, actions action) {
  if (conn < 0) {
    for (uint32_t& v : _action)
      v |= action;
  } else
    _action[conn] |= action;
}

void conflict_manager::__exit() {
  {
    std::lock_guard<std::mutex> lock(_loop_m);
    _exit = true;
    _loop_cv.notify_all();
  }
  if (_thread.joinable())
    _thread.join();
}

/**
 * @brief Returns statistics about the conflict_manager. Those statistics
 * are stored directly in a json tree.
 *
 * @return A nlohmann::json with the statistics.
 */
nlohmann::json conflict_manager::get_statistics() {
  nlohmann::json retval;
  retval["max pending events"] = static_cast<int32_t>(_max_pending_queries);
  retval["max perfdata events"] = static_cast<int32_t>(_max_perfdata_queries);
  retval["loop timeout"] = static_cast<int32_t>(_loop_timeout);
  if (auto lock = std::unique_lock<std::mutex>(_stat_m, std::try_to_lock)) {
    retval["waiting_events"] = static_cast<int32_t>(_fifo.get_events().size());
    retval["events_handled"] = _events_handled;
    retval["sql"] = static_cast<int32_t>(_fifo.get_timeline(sql).size());
    retval["storage"] =
        static_cast<int32_t>(_fifo.get_timeline(storage).size());
    retval["speed"] = fmt::format("{} events/s", _speed);
  }
  return retval;
}

/**
 * @brief Delete the conflict_manager singleton.
 */
int32_t conflict_manager::unload(stream_type type) {
  if (!_singleton) {
    log_v2::sql()->info("conflict_manager: already unloaded.");
    return 0;
  } else {
    uint32_t count = --_singleton->_ref_count;
    int retval;
    if (count == 0) {
      __exit();
      retval = _fifo.get_acks(type);
      {
        std::lock_guard<std::mutex> lck(_init_m);
        _state = finished;
        delete _singleton;
        _singleton = nullptr;
      }
      log_v2::sql()->info(
          "conflict_manager: no more user of the conflict manager.");
    } else {
      log_v2::sql()->info(
          "conflict_manager: still {} stream{} using the conflict manager.",
          count, count > 1 ? "s" : "");
      retval = _fifo.get_acks(type);
      log_v2::sql()->info(
          "conflict_manager: still {} events handled but not acknowledged.",
          retval);
    }
    return retval;
  }
}
