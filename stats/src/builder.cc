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

#include "com/centreon/broker/stats/builder.hh"
#include <time.h>
#include <unistd.h>
#include <asio.hpp>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/endpoint.hh"
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/broker/processing/thread.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
builder::builder() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
builder::builder(builder const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
builder::~builder() throw() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
builder& builder::operator=(builder const& right) {
  if (this != &right) {
    _data = right._data;
    _root = right._root;
  }
  return (*this);
}

/**
 *  Get and build statistics.
 *
 *  @param[in,out] srz  The serializer to use to serialize data.
 */
void builder::build() {
  // Cleanup.
  _data.clear();
  json11::Json::object object;

  // General.
  {
    object["version"] = CENTREON_BROKER_VERSION;
    object["pid"] = getpid();
    object["now"] = std::to_string(::time(nullptr));

    std::string asio_version{std::to_string(ASIO_VERSION / 100000)};
    asio_version.append(".")
        .append(std::to_string(ASIO_VERSION / 100 % 1000))
        .append(".")
        .append(std::to_string(ASIO_VERSION % 100));
    object["asio_version"] = asio_version;
  }

  // Engine.
  {

  }

  // Mysql manager.
  {
    std::map<std::string, std::string> stats(
        mysql_manager::instance().get_stats());
    json11::Json::object subtree;
    for (std::pair<std::string, std::string> const& p : stats)
      subtree[p.first] = p.second;
    object["mysql manager"] = subtree;
  }

  // Modules.
  config::applier::modules& mod_applier(config::applier::modules::instance());
  for (config::applier::modules::iterator it(mod_applier.begin()),
       end(mod_applier.end());
       it != end; ++it) {
    json11::Json::object subtree;
    subtree["state"] = "loaded";
    subtree["size"] =
        std::to_string(misc::filesystem::file_size(it->first)) + "B";
    object["module" + it->first] = subtree;
  }

  // Endpoint applier.
  config::applier::endpoint& endp_applier(
      config::applier::endpoint::instance());

  // Print endpoints.
  {
    bool locked(endp_applier.endpoints_mutex().try_lock_for(
        std::chrono::milliseconds(100)));
    try {
      if (locked)
        for (config::applier::endpoint::iterator
                 it(endp_applier.endpoints_begin()),
             end(endp_applier.endpoints_end());
             it != end; ++it) {
          json11::Json::object p;
          std::string endpoint_name =
              _generate_stats_for_endpoint(it->second, p);
          object[endpoint_name] = p;
        }
      else
        _data.append(
            "inputs=could not fetch list, configuration update in progress "
            "?\n");
    } catch (...) {
      if (locked)
        endp_applier.endpoints_mutex().unlock();
      throw;
    }
    if (locked)
      endp_applier.endpoints_mutex().unlock();
  }

  _root = object;
  std::string buffer;
  _root.dump(buffer);
  _data.insert(0, buffer);
}

/**
 *  Get data buffer.
 *
 *  @return The statistics buffer.
 */
std::string const& builder::data() const throw() {
  return (_data);
}

/**
 *  Get the properties tree.
 *
 *  @return The statistics tree.
 */
json11::Json const& builder::root() const throw() {
  return (_root);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Generate statistics for an endpoint.
 *
 *  @param[in]  fo     Failover thread of the endpoint.
 *  @param[out] tree   Properties for this tree.
 *
 *  @return            Name of the endpoint.
 */
std::string builder::_generate_stats_for_endpoint(processing::bthread* fo,
    json11::Json::object& tree) {
  // Header.
  std::string endpoint = std::string("endpoint ") + fo->get_name();

  // Add memory and queue file.
  tree["queue_file_path"] =
      com::centreon::broker::multiplexing::muxer::queue_file(fo->get_name());
  tree["memory_file_path"] =
      com::centreon::broker::multiplexing::muxer::memory_file(fo->get_name());

  // Gather statistic.
  fo->stats(tree);

  return endpoint;
}
