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

#include "com/centreon/broker/correlation/connector.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Constructor.
 *
 *  @param[in]     correlation_file  Correlation file.
 *  @param[in]     passive           True if endpoint must be in passive
 *                                   mode.
 *  @param[in,out] cache             Endpoint persistent cache.
 */
connector::connector(
             std::string const& correlation_file,
             bool passive,
             std::shared_ptr<persistent_cache> cache)
  : io::endpoint{false},
    _cache{cache},
    _correlation_file{correlation_file},
    _passive{passive} {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other)
  : io::endpoint(other),
    _cache(other._cache),
    _correlation_file(other._correlation_file),
    _passive(other._passive) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _cache = other._cache;
    _correlation_file = other._correlation_file;
    _passive = other._passive;
  }
  return (*this);
}

/**
 *  Open a stream.
 *
 *  @return A newly opened stream.
 */
std::shared_ptr<io::stream> connector::open() {
  // XXX : passive stream
  return std::make_shared<stream>(_correlation_file, _cache, true, _passive);
}
