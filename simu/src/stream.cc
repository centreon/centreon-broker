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
#include "com/centreon/broker/simu/stream.hh"
#include <sstream>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/simu/luabinding.hh"

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
stream::stream(std::string const& lua_script,
               std::map<std::string, misc::variant> const& conf_params) {
  _luabinding = new luabinding(lua_script, conf_params);
}

/**
 *  Destructor.
 */
stream::~stream() {
  delete _luabinding;
}

/**
 *  Write to the connector.
 *
 *  @param[out] d         The data to write to the output.
 *
 *  @return The number of events to acknowledge.
 */
int stream::write(std::shared_ptr<io::data> const& d) {
  (void)d;
  throw exceptions::shutdown() << "cannot write from simu connector";
  return 0;
}

/**
 *  Read from the connector.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  return _luabinding->read(d);
}
