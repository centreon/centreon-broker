/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <vector>
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/stats/parser.hh"
#include "com/centreon/broker/stats/worker_pool.hh"

using namespace com::centreon::broker;

// Load count.
static uint32_t instances(0);

stats::worker_pool pool;

extern "C" {
/**
 *  Module version symbol. Used to check for version mismatch.
 */
char const* broker_module_version = CENTREON_BROKER_VERSION;

/**
 *  Module deinitialization routine.
 */
void broker_module_deinit() {
  // Decrement instance number.
  if (!--instances)
    pool.cleanup();
}

/**
 *  Module initialization routine.
 *
 *  @param[in] arg Configuration argument.
 */
void broker_module_init(void const* arg) {
  // Increment instance number.
  if (!instances++) {
    // Stats module.
    log_v2::sql()->info("stats: module for Centreon Broker {}",
                        CENTREON_BROKER_VERSION);

    // Check that stats are enabled.
    config::state const& base_cfg(*static_cast<config::state const*>(arg));
    bool loaded(false);
    std::map<std::string, std::string>::const_iterator it(
        base_cfg.params().find("stats"));
    if (it != base_cfg.params().end()) {
      try {
        // Parse configuration.
        std::vector<std::string> stats_cfg;
        {
          stats::parser p;
          p.parse(stats_cfg, it->second);
        }

        // File configured, load stats engine.
        for (std::vector<std::string>::const_iterator it = stats_cfg.begin(),
                                                      end = stats_cfg.end();
             it != end; ++it) {
          pool.add_worker(*it);
        }
        loaded = true;
      } catch (std::exception const& e) {
        log_v2::stats()->info("stats: engine loading failure: {}", e.what());
      } catch (...) {
        log_v2::stats()->info("stats: engine loading failure");
      }
    }
    if (!loaded) {
      pool.cleanup();
      log_v2::stats()->info(
          "stats: invalid stats "
          "configuration, stats engine is NOT loaded");
    }
  }
}
}
