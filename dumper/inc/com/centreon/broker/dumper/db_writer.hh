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

#ifndef CCB_DUMPER_DB_WRITER_HH
#  define CCB_DUMPER_DB_WRITER_HH

#  include <list>
#  include "com/centreon/broker/mysql.hh"
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace           dumper {
  namespace         entries {
    class           ba;
    class           ba_type;
    class           boolean;
    class           host;
    class           kpi;
    class           organization;
    class           service;
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
    bool            read(std::shared_ptr<io::data>& d, time_t deadline);
    int             write(std::shared_ptr<io::data> const& d);

  private:
                    db_writer(db_writer const& other);
    db_writer&      operator=(db_writer const& other);
    void            _commit();
    template        <typename T>
    void            _store_objects(
                      mysql& ms,
                      std::list<T> const& l,
                      char const* id_name,
                      unsigned int (T::* id_member));

    database_config _db_cfg;
    bool            _full_dump;
    std::list<entries::ba_type>
                    _ba_types;
    std::list<entries::ba>
                    _bas;
    std::list<entries::boolean>
                    _booleans;
    std::list<entries::host>
                    _hosts;
    std::list<entries::kpi>
                    _kpis;
    std::list<entries::organization>
                    _organizations;
    std::list<entries::service>
                    _services;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_WRITER_HH
