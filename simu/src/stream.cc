/*
** Copyright 2017 Centreon
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
#include <sstream>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/simu/luabinding.hh"
#include "com/centreon/broker/simu/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::simu;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] addr                    Address to connect to
 *  @param[in] port                    port
 */
stream::stream(
          std::string const& lua_script,
          QMap<QString, QVariant> const& conf_params,
          misc::shared_ptr<persistent_cache> const& cache)
  : _cache(cache) {
  _luabinding = new luabinding(lua_script, conf_params, _cache);
}

/**
 *  Destructor.
 */
stream::~stream() {
  delete _luabinding;
}

/**
 *  Read from the connector.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (exceptions::shutdown() << "cannot read from simu generic connector");
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(misc::shared_ptr<io::data> const& data) {
  if (!validate(data, "simu"))
    return 0;

  // Give data to cache.
  _cache.write(data);

  return _luabinding->write(data);
}
