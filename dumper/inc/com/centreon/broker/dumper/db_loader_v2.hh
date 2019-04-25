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

#ifndef CCB_DUMPER_DB_LOADER_V2_HH
#  define CCB_DUMPER_DB_LOADER_V2_HH

#  include "com/centreon/broker/mysql.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           dumper {
  // Forward declaration.
  namespace         entries {
    class           state;
  }

  /**
   *  @class db_loader_v2 db_loader_v2.hh "com/centreon/broker/dumper/db_loader_v2.hh"
   *  @brief Load entries from SQL database with 2.x schema.
   *
   *  Perform SELECT on SQL database to retrieve configuration entries.
   */
  class             db_loader_v2 {
  public:
                    db_loader_v2(mysql& ms);
                    ~db_loader_v2();
    void            load(entries::state& state, unsigned int poller_id);

  private:
                    db_loader_v2(db_loader_v2 const& other);
    db_loader_v2&   operator=(db_loader_v2 const& other);
    void            _load_bas();
    void            _load_booleans();
    void            _load_kpis();
    void            _load_hosts();
    void            _load_services();

    mysql&          _mysql;
    unsigned int    _poller_id;
    entries::state* _state;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_LOADER_V2_HH
