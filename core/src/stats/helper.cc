/*
** Copyright 2020-2021 Centreon
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

#include "com/centreon/broker/stats/helper.hh"

#include <asio.hpp>
#include <json11.hpp>

#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/endpoint.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/broker/processing/endpoint.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

/**
 * @brief Several generic informations returned in statistics.
 *
 * @param object
 */
void com::centreon::broker::stats::get_generic_stats(
    json11::Json::object& object) noexcept {
  object["version"] = CENTREON_BROKER_VERSION;
  object["pid"] = getpid();
  object["now"] = std::to_string(::time(nullptr));

  std::string asio_version(fmt::format("{}.{}.{}", ASIO_VERSION / 100000,
                                       ASIO_VERSION / 100 % 1000,
                                       ASIO_VERSION % 100));

  object["asio_version"] = asio_version;
  json11::Json::object pool;
  pool["size"] = static_cast<int32_t>(pool::instance().get_pool_size());
  pool["latency"] = "";
  object["thread_pool"] = pool;
}

/**
 * @brief Fill object with the mysql connectors data.
 *
 * @param object The json object to fill
 */
void com::centreon::broker::stats::get_mysql_stats(
    json11::Json::object& object) noexcept {
  std::map<std::string, std::string> stats(
      mysql_manager::instance().get_stats());
  for (auto it = stats.begin(), end = stats.end(); it != end; ++it)
    object[it->first] = it->second;
}

void com::centreon::broker::stats::get_loaded_module_stats(
    std::vector<json11::Json::object>& object) noexcept {
  config::applier::modules& mod_applier(
      config::applier::state::instance().get_modules());

  std::lock_guard<std::mutex> lock(mod_applier.module_mutex());

  for (config::applier::modules::iterator it = mod_applier.begin(),
                                          end = mod_applier.end();
       it != end; ++it) {
    json11::Json::object subtree;
    subtree["name"] = it->first;
    subtree["state"] = "loaded";
    subtree["size"] =
        std::to_string(misc::filesystem::file_size(it->first)) + "B";
    object.emplace_back(subtree);
  }
}

/**
 * @brief Fill object with each endpoint information.
 *
 * @param object The json object to fill
 *
 * @return A boolean telling the mutex could be locked to get informations.
 */
bool stats::get_endpoint_stats(std::vector<json11::Json::object>& object) {
  // Endpoint applier.
  if (!config::applier::endpoint::loaded())
    return true;

  config::applier::endpoint& endp_applier(
      config::applier::endpoint::instance());

  // Print endpoints.
  {
    bool locked(endp_applier.endpoints_mutex().try_lock_for(
        std::chrono::milliseconds(100)));
    try {
      if (locked)
        for (auto it(endp_applier.endpoints_begin()),
             end(endp_applier.endpoints_end());
             it != end; ++it) {
          json11::Json::object subtree;
          subtree["name"] = it->second->get_name();
          subtree["queue_file_path"] =
              com::centreon::broker::multiplexing::muxer::queue_file(
                  it->second->get_name());
          subtree["memory_file_path"] =
              com::centreon::broker::multiplexing::muxer::memory_file(
                  it->second->get_name());
          it->second->stats(subtree);
          object.emplace_back(subtree);
        }
    } catch (...) {
      if (locked)
        endp_applier.endpoints_mutex().unlock();
      throw;
    }
    if (locked) {
      endp_applier.endpoints_mutex().unlock();
      return true;
    }

    return false;
  }
}
