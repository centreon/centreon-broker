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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;
using namespace com::centreon::broker::storage;

conflict_manager* conflict_manager::_singleton = nullptr;
std::mutex conflict_manager::_init_m;
std::condition_variable conflict_manager::_init_cv;

void (conflict_manager::*const conflict_manager::_neb_processing_table[])(
    std::shared_ptr<io::data>) = {
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
      _still_pending_events{0},
      _loop_duration{},
      _speed{},
      _ref_count{0},
      _oldest_timestamp{std::numeric_limits<time_t>::max()} {
  log_v2::sql()->debug("conflict_manager: class instanciation");
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
  int count = 0;

  std::unique_lock<std::mutex> lk(_init_m);

  for (;;) {
    /* The loop is waiting for 1s or for _mysql to be initialized */
    if (_init_cv.wait_for(lk, std::chrono::seconds(1),
                          [&]() { return _singleton != nullptr; })) {
      std::lock_guard<std::mutex> lk(_singleton->_loop_m);
      _singleton->_store_in_db = store_in_db;
      _singleton->_rrd_len = rrd_len;
      _singleton->_interval_length = interval_length;
      _singleton->_max_perfdata_queries = queries_per_transaction;
      _singleton->_ref_count++;
      _singleton->_thread =
          std::move(std::thread(&conflict_manager::_callback, _singleton));
      return true;
    }
    count++;
    log_v2::sql()->info(
        "conflict_manager: Waiting for the sql stream initialization for {} "
        "seconds",
        count);
  }
  return false;
}

void conflict_manager::init_sql(database_config const& dbcfg,
                                uint32_t loop_timeout,
                                uint32_t instance_timeout) {
  log_v2::sql()->debug("conflict_manager: sql stream initialization");
  std::lock_guard<std::mutex> lk(_init_m);
  _singleton = new conflict_manager(dbcfg, loop_timeout, instance_timeout);
  _singleton->_action.resize(_singleton->_mysql.connections_count());
  _init_cv.notify_all();
  _singleton->_ref_count++;
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
    throw exceptions::msg()
        << "could not get list of deleted instances: " << e.what();
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
      throw exceptions::msg()
          << "conflict_manager: could not get the list of outdated instances: "
          << e.what();
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
                        .index_id = res.value_as_u32(0),
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
      throw broker::exceptions::msg()
          << "storage: could not fetch index list from data DB: " << e.what();
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
      throw exceptions::msg()
          << "SQL: could not get the list of host/instance pairs: " << e.what();
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
      throw exceptions::msg()
          << "SQL: could not get the list of hostgroups id: " << e.what();
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
      throw exceptions::msg()
          << "SQL: could not get the list of servicegroups id: " << e.what();
    }
  }

  _cache_svc_cmd.clear();
  _cache_hst_cmd.clear();

  /* metrics => _metric_cache */
  {
    std::lock_guard<std::mutex> lock(_metric_cache_m);
    _metric_cache.clear();

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
        _metric_cache[{res.value_as_u32(1), res.value_as_str(2)}] = info;
      }
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "conflict_manager: could not get the list of metrics: "
          << e.what();
    }
  }
}

void conflict_manager::update_metric_info_cache(uint32_t index_id,
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
    it->second.metric_id = metric_id;
  }
}

/**
 *  The main loop of the conflict_manager
 */
void conflict_manager::_callback() {
  try {
    _load_caches();
  } catch (std::exception const& e) {
    logging::error(logging::high) << "error while loading caches: " << e.what();
    _broken = true;
  }

  do {
    /* Are there index_data to remove? */
    try {
      _check_deleted_index();
    } catch (std::exception const& e) {
      logging::error(logging::high)
          << "conflict_manager: error while checking deleted indexes: "
          << e.what();
      _broken = true;
      break;
    }
    try {
      while (!_should_exit()) {
        /* Time to send perfdatas to rrd ; no lock needed, it is this thread
         * that fill this queue. */
        _insert_perfdatas();

        log_v2::sql()->trace(
            "conflict_manager: main loop initialized with a timeout of {} "
            "seconds.",
            _loop_timeout);

        std::chrono::system_clock::time_point now0 =
            std::chrono::system_clock::now();

        int32_t count = 0;
        int32_t timeout = 0;
        int32_t timeout_limit = _loop_timeout * 1000;

        /* This variable is incremented 1000 by 1000 and represents
         * milliseconds. Each time the duration reaches this value, we make
         * stuffs. We make then a timer cadenced at 1000ms. */
        int32_t duration = 1000;

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
          auto* tpl = _fifo.first_event();
          if (!tpl) {
            // We wait for a tuple only if it was impossible to get it
            // immediatly
            tpl = _fifo.first_event_wait(std::chrono::seconds(1));
            if (!tpl) {
              log_v2::sql()->trace(
                  "conflict_manager: timeout reached while waiting for "
                  "events.");
              std::lock_guard<std::mutex> lk(_stat_m);
              _still_pending_events = _fifo.get_events().size();
              _loop_duration = 0;
              _speed = 0;
              break;
            } else
              log_v2::sql()->trace(
                  "conflict_manager: new events to send to the database.");
          }
          std::shared_ptr<io::data>& d = std::get<0>(*tpl);
          uint32_t type{d->type()};
          uint16_t cat{io::events::category_of_type(type)};
          uint16_t elem{io::events::element_of_type(type)};
          if (std::get<1>(*tpl) == sql && cat == io::events::neb)
            (this->*(_neb_processing_table[elem]))(d);
          else if (std::get<1>(*tpl) == storage && cat == io::events::neb &&
                   type == neb::service_status::static_type())
            _storage_process_service_status(d);
          else
            log_v2::sql()->trace(
                "conflict_manager: event of type {} thrown away ; no need to "
                "store it in the database.",
                type);

          ++count;
          *std::get<2>(*tpl) = true;
          _fifo.pop();

          std::chrono::system_clock::time_point now1 =
              std::chrono::system_clock::now();

          timeout =
              std::chrono::duration_cast<std::chrono::milliseconds>(now1 - now0)
                  .count();

          /* Get some stats each second */
          if (timeout >= duration) {
            /* If there are too many perfdata to send, let's send them... */
            if (_perfdata_queue.size() > _max_perfdata_queries)
              _insert_perfdatas();

            do {
              duration += 1000;
            } while (timeout > duration);

            std::lock_guard<std::mutex> lk(_stat_m);
            _still_pending_events = _fifo.get_events().size();
            _loop_duration = timeout;
            _speed = (count * 1000.0) / _loop_duration;
          }
        }

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

        std::chrono::system_clock::time_point now2 =
            std::chrono::system_clock::now();
        /* Get some stats */
        {
          std::lock_guard<std::mutex> lk(_stat_m);
          _still_pending_events = _fifo.get_events().size();
          _loop_duration =
              std::chrono::duration_cast<std::chrono::milliseconds>(now2 - now0)
                  .count();
          if (_loop_duration > 0)
            _speed = count * 1000.0 / _loop_duration;
          else
            _speed = 0;
        }
      }
    } catch (std::exception const& e) {
      logging::error(logging::high)
          << "conflict_manager: error in the main loop: " << e.what();
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
    throw exceptions::msg() << "conflict_manager: events loop interrupted";

  log_v2::sql()->trace(
      "conflict_manager: send_event category:{}, element:{} from {}",
      e->type() >> 16, e->type() & 0xffff, c == 0 ? "sql" : "storage");

  return _fifo.push(c, e);
}

/**
 *  This method is called from the stream and returns how many events should
 *  be released. By the way, it removed those objects from the queue.
 *
 * @param c
 *
 * @return the number of events to ack.
 */
int32_t conflict_manager::get_acks(stream_type c) {
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
 *  The main objective of this method is to commit queries sent to the db.
 *  When the commit is done (all the connections commit), we count how
 *  many events can be acknowledged. So we can also update the number of pending
 *  events.
 */
void conflict_manager::_finish_actions() {
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

json11::Json::object conflict_manager::get_statistics() {
  json11::Json::object retval;
  std::lock_guard<std::mutex> lk(_stat_m);
  retval["max pending events"] = static_cast<int32_t>(_max_pending_queries);
  retval["max perfdata events"] = static_cast<int32_t>(_max_perfdata_queries);
  retval["loop timeout"] = static_cast<int32_t>(_loop_timeout);
  retval["pending events"] = _still_pending_events;
  retval["sql"] = static_cast<int32_t>(_fifo.get_timeline(sql).size());
  retval["storage"] = static_cast<int32_t>(_fifo.get_timeline(storage).size());
  retval["stats interval"] = fmt::format("{} ms", _loop_duration);
  retval["speed"] = fmt::format("{} events/s", _speed);
  return retval;
}

/**
 * @brief Delete the conflict_manager singleton.
 */
void conflict_manager::unload() {
  if (!_singleton)
    log_v2::sql()->info("conflict_manager: already unloaded.");
  else {
    uint32_t count = --_singleton->_ref_count;
    if (count == 0) {
      _singleton->__exit();
      delete _singleton;
      _singleton = nullptr;
      log_v2::sql()->info(
          "conflict_manager: no more user of the conflict manager.");
    } else {
      log_v2::sql()->info(
          "conflict_manager: still {} stream{} using the conflict manager.",
          count, count > 1 ? "s" : "");
    }
  }
}
