/*
** Copyright 2013-2015,2017 Centreon
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

#include "com/centreon/broker/stats/builder.hh"
#include <time.h>
#include <unistd.h>
#include <asio.hpp>
#include <iomanip>
#include <sstream>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/endpoint.hh"
#include "com/centreon/broker/io/properties.hh"
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
void builder::build(serializer const& srz) {
  // Cleanup.
  _data.clear();
  _root = io::properties();

  // General.
  {
    _root.add_property(
        "version",
        io::property("version", misc::string::get(CENTREON_BROKER_VERSION)));
    _root.add_property("pid", io::property("pid", misc::string::get(getpid())));
    _root.add_property("now",
                       io::property("now", misc::string::get(::time(nullptr))));

    std::string asio_version{std::to_string(ASIO_VERSION / 100000)};
    asio_version.append(".")
        .append(std::to_string(ASIO_VERSION / 100 % 1000))
        .append(".")
        .append(std::to_string(ASIO_VERSION % 100));
    _root.add_property("asio_version", asio_version);
  }

  // Mysql manager.
  {
    std::map<std::string, std::string> stats(
        mysql_manager::instance().get_stats());
    io::properties subtree;
    for (std::pair<std::string, std::string> const& p : stats)
      subtree.add_property(p.first, io::property(p.first, p.second));
    _root.add_child(subtree, std::string("mysql manager"));
  }

  // Modules.
  config::applier::modules& mod_applier(config::applier::modules::instance());
  for (config::applier::modules::iterator it(mod_applier.begin()),
       end(mod_applier.end());
       it != end; ++it) {
    io::properties subtree;
    subtree.add_property("state", io::property("state", "loaded"));
    subtree.add_property(
        "size",
        io::property(
            "size",
            misc::string::get(misc::filesystem::file_size(it->first)) + "B"));
    _root.add_child(subtree, std::string("module " + it->first));
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
          io::properties p;
          std::string endpoint_name =
              _generate_stats_for_endpoint(it->second, p);
          _root.add_child(p, endpoint_name);
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

  std::string buffer;
  srz.serialize(buffer, _root);
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
io::properties const& builder::root() const throw() {
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
                                                  io::properties& tree) {
  // Header.
  std::string endpoint = std::string("endpoint ") + fo->get_name();

  // Add memory and queue file.
  tree.add_property(
      "queue_file_path",
      io::property("queue_file_path",
                   com::centreon::broker::multiplexing::muxer::queue_file(
                       fo->get_name())));
  tree.add_property(
      "memory_file_path",
      io::property("memory_file_path",
                   com::centreon::broker::multiplexing::muxer::memory_file(
                       fo->get_name())));

  // Gather statistic.
  fo->stats(tree);

  return (endpoint);
}
