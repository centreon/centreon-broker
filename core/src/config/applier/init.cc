/*
** Copyright 2011-2013, 2021 Centreon
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

#include "com/centreon/broker/config/applier/init.hh"
#include <condition_variable>
#include <cstdlib>
#include <memory>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/broker/time/timezone_manager.hh"

using namespace com::centreon::broker;

std::atomic<config::applier::applier_state> config::applier::mode{not_started};

/**
 * @brief Load necessary structures. It initializes exactly the same structures
 * as init(const config::state& conf) just with detailed parameters.
 *
 * @param n_thread number of threads in the pool.
 * @param name The broker name to give to this cbd instance.
 */
void config::applier::init(size_t n_thread, const std::string& name) {
  set_conflict_manager_initialized(false);
  // Load singletons.
  pool::load(n_thread);
  stats::center::load();
  config::applier::state::load();
  multiplexing::engine::load();
  io::protocols::load();
  io::events::load();
  config::applier::endpoint::load();
  mode = initialized;
}

static std::condition_variable conflict_manager_cv;
static std::mutex conflict_manager_m;
static bool conflict_manager_initialized{false};

void config::applier::set_conflict_manager_initialized(bool initialized) {
  std::unique_lock<std::mutex> lck(conflict_manager_m);
  conflict_manager_initialized = initialized;
  conflict_manager_cv.notify_all();
}

bool config::applier::wait_for_conflict_manager() {
  std::unique_lock<std::mutex> lck(conflict_manager_m);
  conflict_manager_initialized = true;
  conflict_manager_cv.wait(
      lck, [&] { return conflict_manager_initialized || mode == finished; });
  return mode == initialized;
}

/**
 *  Unload necessary structures.
 */
void config::applier::deinit() {
  mode = finished;
  config::applier::endpoint::unload();
  multiplexing::engine::instance().clear();
  multiplexing::engine::unload();
  config::applier::state::unload();
  io::events::unload();
  io::protocols::unload();
  stats::center::unload();
  pool::unload();
}

/**
 * @brief Load necessary structures.
 *
 * @param conf The configuration used to initialize the all.
 */
void config::applier::init(const config::state& conf) {
  init(conf.pool_size(), conf.broker_name());
}
