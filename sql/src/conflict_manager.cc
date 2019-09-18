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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"
#include "com/centreon/broker/storage/index_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

conflict_manager* conflict_manager::_singleton = nullptr;
std::mutex conflict_manager::_init_m;
std::condition_variable conflict_manager::_init_cv;

void (conflict_manager::*const conflict_manager::_neb_processing_table[])() = {
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

conflict_manager::conflict_manager(database_config const& dbcfg)
    : _exit{false},
      _max_pending_queries{dbcfg.get_queries_per_transaction()},
      _pending_queries{0},
      _mysql{dbcfg} {
  _thread = std::move(std::thread(&conflict_manager::_callback, this));
}

/**
 * For the connector that does not initialize the conflict_manager, this
 * function is useful to wait.
 *
 * @param rrd_len
 * @param interval_length
 *
 * @return true if all went OK.
 */
bool conflict_manager::init_storage(uint32_t rrd_len,
                                    uint32_t interval_length) {
  int count = 0;

  std::unique_lock<std::mutex> lk(_init_m);

  while (count < 10) {
    /* The loop is waiting for 1s or for _mysql to be initialized */
    if (_init_cv.wait_for(
            lk, std::chrono::seconds(1), [&]() { return _singleton != nullptr; })) {
      std::lock_guard<std::mutex> lk(conflict_manager::instance()._loop_m);
      conflict_manager::instance()._rrd_len = rrd_len;
      conflict_manager::instance()._interval_length = interval_length;
      return true;
    }
    count++;
  }
  return false;
}

void conflict_manager::init_sql(database_config const& dbcfg) {
  std::lock_guard<std::mutex> lk(_init_m);
  _singleton = new conflict_manager(dbcfg);
  _init_cv.notify_all();
  _singleton->_action.resize(_singleton->_mysql.connections_count());
}

void conflict_manager::_load_caches() {
  // Fill index cache.
  {
    std::lock_guard<std::mutex> lk(_loop_m);
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
        logging::debug(logging::high)
            << "storage: loaded index " << info.index_id << " of (" << host_id
            << ", " << service_id << ")";
        _index_cache[{host_id, service_id}] = info;

        // Create the metric mapping.
        std::shared_ptr<storage::index_mapping> im{std::make_shared<storage::index_mapping>(info.index_id, host_id, service_id)};
        multiplexing::publisher pblshr;
        pblshr.write(im);
      }
    } catch (std::exception const& e) {
      throw broker::exceptions::msg()
          << "storage: could not fetch index list from data DB: " << e.what();
    }
  }

}

/**
 *  The main loop of the conflict_manager
 */
void conflict_manager::_callback() {
  _load_caches();
  while (!_should_exit()) {
    std::unique_lock<std::mutex> lk(_loop_m);

    /* The loop is waiting for 1s or for _pending_queries to be equal to
     * _max_pending_queries */
    if (_loop_cv.wait_for(lk, std::chrono::seconds(1), [this]() {
          return _pending_queries == _max_pending_queries;
        }))
      logging::info(logging::low)
          << "conflict_manager: sending max pending queries ("
          << _max_pending_queries << ").";
    else
      logging::info(logging::low)
          << "conflict_manager: timeout reached - sending " << _pending_queries
          << " queries.";

    while (!_events.empty()) {
      std::shared_ptr<io::data> d{std::get<0>(_events.front())};
      uint32_t type{d->type()};
      uint16_t cat{io::events::category_of_type(type)};
      uint16_t elem{io::events::element_of_type(type)};
      if (std::get<1>(_events.front()) == sql) {
        if (cat == io::events::neb)
          (this->*(_neb_processing_table[elem]))();
      }
      else if (type == neb::service_status::static_type())
        _storage_process_service_status();
      else {
        logging::info(logging::low)
          << "conflict_manager: event of type " << type << "throw away";
        *std::get<2>(_events.front()) = true;
        _events.pop_front();
      }
    }
  }
  _mysql.commit();
}

/**
 *  Accessor to the boolean telling if the loop should stop.
 *
 * @return True if the loop must be interrupted, false otherwise.
 */
bool conflict_manager::_should_exit() const {
  std::lock_guard<std::mutex> lock(_loop_m);
  return _exit;
}

void conflict_manager::send_event(conflict_manager::stream_type c,
                                  std::shared_ptr<io::data> const& e) {
  std::lock_guard<std::mutex> lk(_loop_m);
  _pending_queries++;
  _timeline[c].push_back(false);
  _events.push_back(std::make_tuple(e, c, &_timeline[c].back()));
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
  int32_t retval = 0;
  while (!_timeline[c].empty() && _timeline[c].front()) {
    _timeline[c].pop_front();
    retval++;
  }
  _pending_queries -= retval;
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
void conflict_manager::_finish_action(int32_t conn, actions action) {
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
    _action[conn] |= actions::none;
}
