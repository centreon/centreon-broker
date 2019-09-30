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
 *  Comparaison operator for test purpose.
 *
 *  @param[in] other  Object to compare
 *
 *  @return This object.
 */
bool connector::operator==(const connector& other) {
  if (this != &other) {
    return _db_cfg == other._db_cfg && _rrd_len == other._rrd_len &&
           _rebuild_check_interval == other._rebuild_check_interval &&
           _store_in_data_bin == other._store_in_data_bin;
  }
  return true;
}

/**
 *  Set connection parameters.
 *
 *  @param[in] db_cfg                  Database configuration.
 *  @param[in] rrd_len                 RRD storage length.
 *  @param[in] interval_length         Length of a time unit.
 *  @param[in] rebuild_check_interval  How often the storage endpoint
 *                                     must check for graph rebuild.
 *  @param[in] store_in_data_bin       True to store performance data in
 *                                     the data_bin table.
 */
void connector::connect_to(database_config const& db_cfg,
                           uint32_t rrd_len,
                           uint32_t interval_length,
                           uint32_t rebuild_check_interval,
                           bool store_in_data_bin) {
  _db_cfg = db_cfg;
  _rrd_len = rrd_len;
  _interval_length = interval_length;
  _rebuild_check_interval = rebuild_check_interval;
  _store_in_data_bin = store_in_data_bin;
}

/**
 *  Connect to a DB.
 *
 *  @param[in] cache  Persistent cache is not used by the storage
 *                    module.
 *
 *  @return Storage connection object.
 */
std::shared_ptr<io::stream> connector::open() {
  return std::shared_ptr<io::stream>(
      std::make_shared<stream>(_db_cfg,
                               _rrd_len,
                               _interval_length,
                               _rebuild_check_interval,
                               _store_in_data_bin));
}
