/*
** Copyright 2011-2012,2015 Centreon
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

#include "com/centreon/broker/sql/connector.hh"
#include "com/centreon/broker/sql/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() : io::endpoint(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other)
  : io::endpoint(other),
    _cleanup_check_interval(other._cleanup_check_interval),
    _dbcfg(other._dbcfg),
    _instance_timeout(other._instance_timeout),
    _with_state_events(other._with_state_events) {}

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
    _cleanup_check_interval = other._cleanup_check_interval;
    _dbcfg = other._dbcfg;
    _instance_timeout = other._instance_timeout;
    _with_state_events = other._with_state_events;
  }
  return (*this);
}

/**
 *  Set connection parameters.
 *
 *  @param[in] dbcfg                   Database configuration.
 *  @param[in] cleanup_check_interval  How often the stream must
 *                                     check for cleanup database.
 *  @param[in] instance_timeout        Timeout of instances.
 *  @param[in] with_state_events       Enable state events ?
 */
void connector::connect_to(
                  database_config const& dbcfg,
                  unsigned int cleanup_check_interval,
                  unsigned int instance_timeout,
                  bool with_state_events,
                  bool enable_cmd_cache) {
  _cleanup_check_interval = cleanup_check_interval;
  _dbcfg = dbcfg;
  _instance_timeout = instance_timeout;
  _with_state_events = with_state_events;
  _enable_cmd_cache = enable_cmd_cache;
}

/**
 *  Connect to a DB.
 *
 *  @return SQL connection object.
 */
std::shared_ptr<io::stream> connector::open() {
  return std::shared_ptr<io::stream>(new stream(
                _dbcfg,
                _cleanup_check_interval,
                _instance_timeout,
                _with_state_events));
}
