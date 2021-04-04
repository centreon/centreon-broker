/*
** Copyright 2011-2015,2017 Centreon
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

#include "com/centreon/broker/storage/connector.hh"

#include "com/centreon/broker/storage/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

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
 *  Set connection parameters.
 *
 *  @param[in] rrd_len                 RRD storage length.
 *  @param[in] interval_length         Length of a time unit.
 *  @param[in] rebuild_check_interval  How often the storage endpoint
 *                                     must check for graph rebuild.
 *  @param[in] store_in_data_bin       True to store performance data in
 *                                     the data_bin table.
 */
void connector::connect_to(database_config const& dbcfg,
                           uint32_t rrd_len,
                           uint32_t interval_length,
                           uint32_t rebuild_check_interval,
                           bool store_in_data_bin) {
  _dbcfg = dbcfg;
  _rrd_len = rrd_len;
  _interval_length = interval_length;
  _rebuild_check_interval = rebuild_check_interval;
  _store_in_data_bin = store_in_data_bin;
}

/**
 * @brief Open a connection object to the database.
 *
 * @return Storage connection object.
 */
std::unique_ptr<io::stream> connector::open() {
  return std::unique_ptr<stream>(new stream(_dbcfg, _rrd_len, _interval_length,
                                            _rebuild_check_interval,
                                            _store_in_data_bin));
}
