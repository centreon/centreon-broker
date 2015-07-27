/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_DUMPER_DB_LOADER_HH
#  define CCB_DUMPER_DB_LOADER_HH

#  include <memory>
#  include "com/centreon/broker/database.hh"
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           dumper {
  // Forward declaration.
  namespace           entries {
    class             state;
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
    void            _load_ba_types();
    void            _load_bas();
    void            _load_kpis();

    std::auto_ptr<database>
                    _db;
    database_config _db_cfg;
    unsigned int    _poller_id;
    entries::state* _state;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_LOADER_HH
