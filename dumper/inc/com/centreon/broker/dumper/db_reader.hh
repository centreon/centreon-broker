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

#ifndef CCB_DUMPER_DB_READER_HH
#  define CCB_DUMPER_DB_READER_HH

#  include <QString>
#  include <string>
#  include "com/centreon/broker/dumper/db_loader.hh"
#  include "com/centreon/broker/dumper/entries/state.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"

CCB_BEGIN()

// Forward declaration.
class               database_config;

namespace           dumper {
  /**
   *  @class db_reader db_reader.hh "com/centreon/broker/dumper/db_reader.hh"
   *  @brief Dump DB configuration.
   *
   *  Read configuration from database and dumps it.
   */
  class             db_reader : public io::stream {
  public:
                    db_reader(
                      std::string const& name,
                      database_config const& db_cfg);
                    ~db_reader();
    void            read(misc::shared_ptr<io::data>& d);
    unsigned int    write(misc::shared_ptr<io::data> const& d);
    void            process(bool in = false, bool out = true);

  private:
                    db_reader(db_reader const& other);
    db_reader&      operator=(db_reader const& other);
    void            _sync_cfg_db(unsigned int poller_id);
    void            _update_cfg_db(unsigned int poller_id);

    umap<unsigned int, entries::state>
                    _cache;
    db_loader       _loader;
    QString         _name;
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_READER_HH
