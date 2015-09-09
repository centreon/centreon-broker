/*
** Copyright 2015 Centreon
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

#include "com/centreon/broker/neb/node_events_connector.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Constructor.
 *
 *  @param[in]     name              Endpoint name, to properly respond
 *                                   to external commands.
 *  @param[in,out] cache             Endpoint persistent cache.
 *  @param[in]     config_file       Configuration file with downtimes
 *                                   and timeperiods.
 */
node_events_connector::node_events_connector(
                         std::string const& name,
                         misc::shared_ptr<persistent_cache> cache,
                         std::string const& config_file)
  : io::endpoint(false),
    _cache(cache),
    _config_file(config_file),
    _name(name) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
node_events_connector::node_events_connector(node_events_connector const& other)
  : io::endpoint(other),
    _cache(other._cache),
    _config_file(other._config_file),
    _name(other._name) {}

/**
 *  Destructor.
 */
node_events_connector::~node_events_connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
node_events_connector& node_events_connector::operator=(
                         node_events_connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _cache = other._cache;
    _config_file = other._config_file;
    _name = other._name;
  }
  return (*this);
}

/**
 *  Open a stream.
 *
 *  @return A newly opened stream.
 */
misc::shared_ptr<io::stream> node_events_connector::open() {
  return (new node_events_stream(_name.c_str(), _cache, _config_file));
}
