/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/connector.hh"

#include "com/centreon/broker/bam/monitoring_stream.hh"
#include "com/centreon/broker/bam/reporting_stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
connector::connector() : io::endpoint(false), _type(bam_monitoring_type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other) : io::endpoint(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Set connection parameters.
 *
 *  @param[in] ext_cmd_file     The external command file to connect to Centreon
 * Engine.
 *  @param[in] db_cfg           Database configuration.
 *  @param[in] storage_db_name  Storage database name.
 *  @param[in] cache            The persistent cache.
 */
void connector::connect_monitoring(std::string const& ext_cmd_file,
                                   database_config const& db_cfg,
                                   std::string const& storage_db_name,
                                   std::shared_ptr<persistent_cache> cache) {
  _type = bam_monitoring_type;
  _ext_cmd_file = ext_cmd_file;
  _db_cfg = db_cfg;
  _cache = cache;
  if (storage_db_name.empty())
    _storage_db_name = db_cfg.get_name();
  else
    _storage_db_name = storage_db_name;
  return;
}

/**
 *  Set reporting connection parameters.
 *
 *  @param[in] db_cfg  Database configuration.
 */
void connector::connect_reporting(database_config const& db_cfg) {
  _type = bam_reporting_type;
  _db_cfg = db_cfg;
  _storage_db_name.clear();
  return;
}

/**
 * @brief Connect to a DB.
 *
 * @return BAM connection object.
 */
std::shared_ptr<io::stream> connector::open() {
  if (_type == bam_reporting_type)
    return std::make_shared<reporting_stream>(_db_cfg);
  else {
    database_config storage_db_cfg(_db_cfg);
    storage_db_cfg.set_name(_storage_db_name);
    auto s = std::make_shared<monitoring_stream>(_ext_cmd_file, _db_cfg,
                                                 storage_db_cfg, _cache);
    s->initialize();
    return s;
  }
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void connector::_internal_copy(connector const& other) {
  _db_cfg = other._db_cfg;
  _storage_db_name = other._storage_db_name;
  _type = other._type;
  _cache = other._cache;
  return;
}
