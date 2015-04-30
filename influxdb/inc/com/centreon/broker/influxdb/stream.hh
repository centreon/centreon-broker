/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_INFLUXDB_STREAM_HH
#  define CCB_INFLUXDB_STREAM_HH

#  include <deque>
#  include <list>
#  include <map>
#  include <memory>
#  include <QSqlDatabase>
#  include <QString>
#  include <utility>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/influxdb/influxdb.hh"
#  include "com/centreon/broker/influxdb/column.hh"
#  include "com/centreon/broker/persistent_cache.hh"
#  include "com/centreon/broker/influxdb/macro_cache.hh"

CCB_BEGIN()

// Forward declaration.
class              database_config;

namespace          influxdb {
  /**
   *  @class stream stream.hh "com/centreon/broker/influxdb/stream.hh"
   *  @brief Influxdb stream.
   *
   *  Insert metrics into influxdb.
   */
  class            stream : public io::stream {
  public:
                   stream(
                     std::string const& user,
                     std::string const& passwd,
                     std::string const& addr,
                     unsigned short port,
                     std::string const& db,
                     unsigned int queries_per_transaction,
                     std::string const& version,
                     std::string const& status_ts,
                     std::vector<column> const& status_cols,
                     std::string const& metric_ts,
                     std::vector<column> const& metric_cols,
                     misc::shared_ptr<persistent_cache> const& cache);
                   ~stream();
    void           process(bool in = false, bool out = true);
    void           read(misc::shared_ptr<io::data>& d);
    void           statistics(io::properties& tree) const;
    void           update();
    unsigned int   write(misc::shared_ptr<io::data> const& d);

  private:
    bool          _process_out;

    // Database parameters
    std::string  _user;
    std::string  _password;
    std::string  _address;
    unsigned short
                 _port;
    std::string  _db;
    unsigned int _queries_per_transaction;

    std::auto_ptr<influxdb>
                 _influx_db;

    // Internal working members
    unsigned int _actual_query;

    // Cache
    macro_cache  _cache;

    // Status members
    std::string    _status;
    mutable QMutex _statusm;
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_STREAM_HH
