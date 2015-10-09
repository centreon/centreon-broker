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

#ifndef CCB_DUMPER_DB_LOADER_HH
#  define CCB_DUMPER_DB_LOADER_HH

#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           dumper {
  // Forward declaration.
  namespace         entries {
    class           state;
  }

  /**
   *  @class db_loader db_loader.hh "com/centreon/broker/dumper/db_loader.hh"
   *  @brief Load entries from SQL database.
   *
   *  Perform SELECT on SQL database to retrieve configuration entries.
   */
  class             db_loader {
  public:
                    db_loader(database_config const& db_cfg);
                    ~db_loader();
    void            load(entries::state& state, unsigned int poller_id);

  private:
                    db_loader(db_loader const& other);
    db_loader&      operator=(db_loader const& other);

    database_config _db_cfg;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_LOADER_HH
