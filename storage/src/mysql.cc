/*
** Copyright 2018 Centreon
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
#include "com/centreon/broker/storage/mysql.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg  Database configuration.
 */
mysql::mysql(database_config const& db_cfg)
  : _db_cfg(db_cfg),
    _version(database::v3) {
  if (mysql_library_init(0, NULL, NULL)) {
    logging::error(logging::high)
      << "storage: unable to initialize the MySQL connector";
    return ;
  }

  for (int i(0); i < db_cfg.get_connections_count(); ++i)
    _thread.push_back(new mysql_thread(
                            db_cfg.get_host(),
                            db_cfg.get_user(),
                            db_cfg.get_password(),
                            db_cfg.get_name(),
                            db_cfg.get_port()));
}
