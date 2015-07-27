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

#ifndef CCB_DUMPER_DB_WRITER_HH
#  define CCB_DUMPER_DB_WRITER_HH

#  include <list>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           dumper {
  // Forward declarations.
  namespace         entries {
    class           ba;
    class           ba_type;
    class           kpi;
    class           organization;
  }

  /**
   *  @class db_writer db_writer.hh "com/centreon/broker/dumper/db_writer.hh"
   *  @brief Write DB configuration.
   *
   *  Retrieve database configuration from db_reader and write it to
   *  a database.
   */
  class             db_writer : public io::stream {
  public:
                    db_writer(database_config const& db_cfg);
                    ~db_writer();
    bool            read(misc::shared_ptr<io::data>& d, time_t deadline);
    unsigned int    write(misc::shared_ptr<io::data> const& d);

  private:
                    db_writer(db_writer const& other);
    db_writer&      operator=(db_writer const& other);
    void            _commit();

    database_config _db_cfg;
    bool            _full_dump;
    std::list<entries::ba_type>
                    _ba_types;
    std::list<entries::ba>
                    _bas;
    std::list<entries::kpi>
                    _kpis;
    std::list<entries::organization>
                    _organizations;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_WRITER_HH
