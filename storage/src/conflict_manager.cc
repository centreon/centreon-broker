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
#include <cassert>
#include <cstring>
#include "com/centreon/broker/database/mysql_result.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/broker/storage/index_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;
using namespace com::centreon::broker::storage;

conflict_manager* conflict_manager::_singleton = nullptr;
std::mutex conflict_manager::_init_m;
std::condition_variable conflict_manager::_init_cv;

int32_t (conflict_manager::*const conflict_manager::_neb_processing_table[])() =
    {nullptr,
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
     &conflict_manager::_process_responsive_instance, };

conflict_manager& conflict_manager::instance() {
  assert(_singleton);
  return *_singleton;
}

conflict_manager::conflict_manager(database_config const& dbcfg,
                                   uint32_t loop_timeout,
                                   uint32_t instance_timeout)
    : _exit{false},
      _loop_timeout{loop_timeout},
      _max_pending_queries{dbcfg.get_queries_per_transaction()},
      _pending_queries{0},
      _mysql{dbcfg},
      _instance_timeout{instance_timeout},
      _still_pending_events{},
      _loop_duration{},
      _speed{},
      _ref_count{0},
      _oldest_timestamp{std::numeric_limits<time_t>::max()} {
  log_v2::sql()->debug("conflict_manager: class instanciation");
  _thread = std::move(std::thread(&conflict_manager::_callback, this));
}

conflict_manager::~conflict_manager() {
  log_v2::sql()->debug("conflict_manager: destruction");
}

/**
 * For the connector that does not initialize the conflict_manager, this
 * function is useful to wait.
 *
 * @param store_in_db
 * @param rrd_len
 * @param interval_length
 *
 * @return true if all went OK.
 */
bool conflict_manager::init_storage(bool store_in_db,
                                    uint32_t rrd_len,
                                    uint32_t interval_length) {
  log_v2::sql()->debug(
      "conflict_manager: storage stream initialization");
  int count = 0;

  std::unique_lock<std::mutex> lk(_init_m);

  for (;;) {
    /* The loop is waiting for 1s or for _mysql to be initialized */
    if (_init_cv.wait_for(lk, std::chrono::seconds(1), [&]() {
          return _singleton != nullptr;
        })) {
      std::lock_guard<std::mutex> lk(_singleton->_loop_m);
      _singleton->_store_in_db = store_in_db;
      _singleton->_rrd_len = rrd_len;
      _singleton->_interval_length = interval_length;
      _singleton->_ref_count++;
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

void conflict_manager::close() {
  log_v2::sql()->debug("conflict_manager: closing the manager");
  conflict_manager::instance().exit();
  std::lock_guard<std::mutex> lk(_init_m);
  delete _singleton;
  _singleton = nullptr;
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
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
        << "conflict_manager: could not get list of deleted instances: "
        << e.what();
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
    }
    catch (std::exception const& e) {
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
        index_info info;
        info.index_id = res.value_as_u32(0);
        uint32_t host_id(res.value_as_u32(1));
        uint32_t service_id(res.value_as_u32(2));
        info.host_name = res.value_as_str(3);
        info.rrd_retention = res.value_as_u32(4);
        if (!info.rrd_retention)
          info.rrd_retention = _rrd_len;
        info.service_description = res.value_as_str(5);
        info.special = (res.value_as_u32(6) == 2);
        info.locked = res.value_as_bool(7);
        logging::debug(logging::high) << "storage: loaded index "
                                      << info.index_id << " of (" << host_id
                                      << ", " << service_id << ")";
        _index_cache[{host_id, service_id}] = info;

        // Create the metric mapping.
        std::shared_ptr<storage::index_mapping> im{
            std::make_shared<storage::index_mapping>(
                info.index_id, host_id, service_id)};
        multiplexing::publisher pblshr;
        pblshr.write(im);
      }
    }
    catch (std::exception const& e) {
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
    }
    catch (std::exception const& e) {
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
    }
    catch (std::exception const& e) {
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
    }
    catch (std::exception const& e) {
      throw exceptions::msg()
          << "SQL: could not get the list of servicegroups id: " << e.what();
    }
  }

  _cache_svc_cmd.clear();
  _cache_hst_cmd.clear();

  /* metrics => _metric_cache */
  {
    _metric_cache.clear();

    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(
        "SELECT "
        "metric_id,index_id,metric_name,unit_name,warn,warn_low,warn_threshold_"
        "mode,crit,crit_low,crit_threshold_mode,min,max,current_value,data_"
        "source_type FROM metrics",
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
    }
    catch (std::exception const& e) {
      throw exceptions::msg()
        << "conflict_manager: could not get the list of metrics: " << e.what();
    }
  }
}

/**
 *  The main loop of the conflict_manager
 */
void conflict_manager::_callback() {
  _load_caches();

  do {
    try {
      while (!_should_exit()) {
        /* Time to send perfdatas to rrd ; no lock needed, it is this thread
         * that
         * fill this queue. */
        _insert_perfdatas();

        logging::info(logging::low)
            << "conflict_manager: main loop initialized with a timeout of "
            << _loop_timeout << " seconds.";

        std::chrono::system_clock::time_point now0 =
            std::chrono::system_clock::now();
        std::unique_lock<std::mutex> lk(_loop_m);

        uint32_t count = 0;
        int32_t timeout = 0;
        int32_t timeout_limit = _loop_timeout * 1000;
        std::chrono::system_clock::time_point previous_time(now0);

        /* Let's wait for some events before entering in the loop */
        if (_loop_cv.wait_for(lk, std::chrono::seconds(1), [this]() {
              return !_events.empty();
            }))
          logging::info(logging::low)
              << "conflict_manager: events to send to the database received.";
        else
          logging::info(logging::low)
              << "conflict_manager: timeout reached while waiting for events.";

        /* During this loop, connectors still fill the queue when they receive
         * new events. To allow that, we have to release the mutex. We have the
         * chance that the queue does not move old objects when it adds new
         * ones. So when we access to events already there, we don't have to
         * maintain the lock.
         * We just set the lock when we have to pop the events (when they are
         * recorded on the DB).
         * The loop is hold by three conditions that are:
         * - events.empty() no more events to treat.
         * - count < _max_pending_queries: we don't want to commit everytimes,
         *   so we keep this count to know if we reached the
         *   _max_pending_queries parameter.
         * - timeout < timeout_limit: If the loop lives to long, we interrupt it
         *   it is necessary for cleanup operations.
         */
        while (!_events.empty() && count < _max_pending_queries &&
               timeout < timeout_limit) {
          std::shared_ptr<io::data> d{std::get<0>(_events.front())};
          lk.unlock();
          uint32_t type{d->type()};
          uint16_t cat{io::events::category_of_type(type)};
          uint16_t elem{io::events::element_of_type(type)};
          if (std::get<1>(_events.front()) == sql && cat == io::events::neb)
            count += (this->*(_neb_processing_table[elem]))();
          else if (std::get<1>(_events.front()) == storage &&
                   type == neb::service_status::static_type())
            count += _storage_process_service_status();
          else {
            logging::info(logging::low) << "conflict_manager: event of type "
                                        << type << " throw away";
            *std::get<2>(_events.front()) = true;
            _events.pop_front();
            count++;
          }
          std::chrono::system_clock::time_point now1 =
              std::chrono::system_clock::now();

          timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
              now1 - now0).count();
          lk.lock();
          if (!_exit && _events.empty()) {
            logging::debug(logging::low) << "conflict_manager: no more events "
                                            "in the loop, let's wait for them";
            /* There is no more events to send to the DB, let's wait for new
             * ones. */
            if (_loop_cv.wait_for(lk, std::chrono::milliseconds(timeout_limit - timeout), [this]() {
                  return _exit || !_events.empty();
                }))
              logging::info(logging::low) << "conflict_manager: new events to "
                                             "send to the database received.";
            else
              logging::info(logging::low) << "conflict_manager: timeout "
                                             "reached while waiting for "
                                             "new events.";
          }

          /* Get some stats every seconds */
          if (std::chrono::duration_cast<std::chrono::milliseconds>(
                  now1 - previous_time).count() > 1000) {
            previous_time = now1;
            std::lock_guard<std::mutex> lk(_stat_m);
            _still_pending_events = _events.size();
            _loop_duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now1 - now0).count();
            if (_loop_duration > 0)
              _speed = (count * 1000.0) / _loop_duration;
            else
              _speed = 0;
          }
        }

        /* Here, just before looping, we commit. */
        _finish_actions();
        if (_pending_queries == 0)
          logging::debug(logging::high)
              << "conflict_manager: acknowledgement - no pending events";
        else
          logging::debug(logging::high)
              << "conflict_manager: acknowledgement - still "
              << _pending_queries << " not acknowledged";

        /* Several checks on the database,  no need to keep the loop mutex */
        lk.unlock();
        /* Are there unresonsive instances? */
        _update_hosts_and_services_of_unresponsive_instances();

        /* Are there index_data to remove? */
        _check_deleted_index();
        lk.lock();
        /* Checks are finished */

        std::chrono::system_clock::time_point now2 =
            std::chrono::system_clock::now();
        /* Get some stats */
        {
          std::lock_guard<std::mutex> lk(_stat_m);
          _still_pending_events = _events.size();
          _loop_duration =
              std::chrono::duration_cast<std::chrono::milliseconds>(now2 - now0)
                  .count();
          if (_loop_duration > 0)
            _speed = (count * 1000.0) / _loop_duration;
          else
            _speed = 0;
        }
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
          << "conflict_manager: error in the main loop: " << e.what();
      if (strstr(e.what(), "server has gone away")) {
        // The case where we must restart the connector.
        throw;
      }
    }
  } while (!_should_exit());
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
  return _exit && _events.empty();
}

/**
 *  Method to send event to the conflict manager.
 *
 * @param c The connector responsible of the event (sql or storage)
 * @param e The event
 *
 * @return The number of events to ack.
 */
void conflict_manager::send_event(conflict_manager::stream_type c,
                                  std::shared_ptr<io::data> const& e) {
  assert(e);
  log_v2::sql()->trace(
      "conflict_manager: send_event category:{}, element:{} from {}",
      e->type() >> 16,
      e->type() & 0xffff,
      c == 0 ? "sql" : "storage");
  std::lock_guard<std::mutex> lk(_loop_m);
  _pending_queries++;
  _timeline[c].push_back(false);
  _events.emplace_back(std::make_tuple(e, c, &_timeline[c].back()));
  _loop_cv.notify_all();
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
  std::lock_guard<std::mutex> lk(_loop_m);
  int32_t retval = _ack[c];
  _ack[c] = 0;
  return retval;
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
  }
  else if (_action[conn] & action) {
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
  for (stream_type c : {sql, storage}) {
    int32_t retval = 0;
    while (!_timeline[c].empty() && _timeline[c].front()) {
      _timeline[c].pop_front();
      retval++;
    }
    _pending_queries -= retval;
    _ack[c] = retval;
  }
  logging::debug(logging::high) << "conflict_manager: still " << _pending_queries
    << " not acknowledged";
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
  }
  else
    _action[conn] |= action;
}

void conflict_manager::exit() {
  {
    std::lock_guard<std::mutex> lock(_loop_m);
    _exit = true;
  }
  if (_thread.joinable())
    _thread.join();
}

json11::Json::object conflict_manager::get_statistics() {
  json11::Json::object retval;
  std::lock_guard<std::mutex> lk(_stat_m);
  retval["pending events"] = _still_pending_events;
  retval["loop duration"] = std::to_string(_loop_duration) + " ms";
  retval["speed"] = std::to_string(_speed) + " events/s";
  return retval;
}

void conflict_manager::_pop_event(
    std::tuple<std::shared_ptr<io::data>, stream_type, bool*>& p) {
  std::lock_guard<std::mutex> lk(_loop_m);
  *std::get<2>(p) = true;
  _events.pop_front();
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
      _singleton->exit();
      delete _singleton;
      _singleton = nullptr;
      log_v2::sql()->info(
          "conflict_manager: no more user of the conflict manager.");
    } else {
      log_v2::sql()->info(
          "conflict_manager: still {} stream{} using the conflict manager.",
          count,
          count > 1 ? "s" : "");
    }
  }
}
