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

#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/dumper/db_loader.hh"
#include "com/centreon/broker/dumper/db_loader_v2.hh"
#include "com/centreon/broker/dumper/db_loader_v3.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg  Database configuration.
 */
db_loader::db_loader(database_config const& db_cfg) : _db_cfg(db_cfg) {}

/**
 *  Destructor.
 */
db_loader::~db_loader() {}

/**
 *  Load entries from database.
 *
 *  @param[out] state      State.
 *  @param[in]  poller_id  Poller ID.
 */
void db_loader::load(entries::state& state, unsigned int poller_id) {
  mysql ms(_db_cfg);
  if (ms.schema_version() == mysql::v2) {
    db_loader_v2 dblv2(ms);
    dblv2.load(state, poller_id);
  }
  else {
    db_loader_v3 dblv3(ms);
    dblv3.load(state, poller_id);
  }
}
