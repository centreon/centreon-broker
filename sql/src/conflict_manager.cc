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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/sql/conflict_manager.hh"
#include "com/centreon/broker/neb/events.hh"

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
 *  For the connector that does not initialize the conflict_manager, this
 *  function is useful to wait.
 *
 * @return true if all went OK.
 */
bool conflict_manager::wait_for_init() {
  int count = 0;

  std::unique_lock<std::mutex> lk(_init_m);

  while (count < 10) {
    /* The loop is waiting for 1s or for _mysql to be initialized */
    if (_init_cv.wait_for(
            lk, std::chrono::seconds(1), [&]() { return _singleton != nullptr; }))
      return true;
    count++;
  }
  return false;
}

void conflict_manager::init(database_config const& dbcfg) {
  std::lock_guard<std::mutex> lk(_init_m);
  _singleton = new conflict_manager(dbcfg);
  _init_cv.notify_all();
}

/**
 *  The main loop of the conflict_manager
 */
void conflict_manager::_callback() {
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
      std::shared_ptr<io::data> d{_events.front().first};
      uint32_t type{d->type()};
      uint16_t cat{io::events::category_of_type(type)};
      uint16_t elem{io::events::element_of_type(type)};
      if (cat == io::events::neb)
        (this->*(_neb_processing_table[elem]))();
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
  _events.push_back({e, &_timeline[c].back()});
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
