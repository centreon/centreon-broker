/*
** Copyright 2011-2017 Centreon
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

#include "com/centreon/broker/influxdb/connector.hh"
#include "com/centreon/broker/influxdb/stream.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

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
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Set connection parameters.
 *
 */
void connector::connect_to(std::string const& user,
                           std::string const& passwd,
                           std::string const& addr,
                           unsigned short port,
                           std::string const& db,
                           uint32_t queries_per_transaction,
                           std::string const& status_ts,
                           std::vector<column> const& status_cols,
                           std::string const& metric_ts,
                           std::vector<column> const& metric_cols,
                           std::shared_ptr<persistent_cache> const& cache) {
  _user = user;
  _password = passwd;
  _addr = addr;
  _port = port, _db = db;
  _queries_per_transaction = queries_per_transaction;
  _status_ts = status_ts;
  _status_cols = status_cols;
  _metric_ts = metric_ts;
  _metric_cols = metric_cols;
  _cache = cache;
}

/**
 * @brief Connect to an influxdb DB.
 *
 * @return An Influxdb connection object.
 */
std::shared_ptr<io::stream> connector::open() {
  return std::make_shared<stream>(
      _user, _password, _addr, _port, _db, _queries_per_transaction, _status_ts,
      _status_cols, _metric_ts, _metric_cols, _cache);
}
