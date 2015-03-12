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

#ifndef CCB_GRAPHITE_STREAM_HH
#  define CCB_GRAPHITE_STREAM_HH

#  include <deque>
#  include <list>
#  include <map>
#  include <memory>
#  include <QSqlDatabase>
#  include <QString>
#  include <utility>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/multiplexing/hooker.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/storage/status.hh"
#  include "com/centreon/broker/graphite/query.hh"

CCB_BEGIN()

// Forward declaration.
class              database_config;

namespace          graphite {
  /**
   *  @class stream stream.hh "com/centreon/broker/graphite/stream.hh"
   *  @brief Graphite stream.
   *
   *  Insert metrics/statuses into graphite.
   */
  class            stream : public io::stream {
  public:
                   stream(
                     std::string const& metric_naming,
                     std::string const& status_naming,
                     std::string const& db_user,
                     std::string const& db_password,
                     std::string const& db_host,
                     unsigned short db_port,
                     unsigned int queries_per_transaction);
                   ~stream();
    void           process(bool in = false, bool out = true);
    void           read(misc::shared_ptr<io::data>& d);
    void           statistics(io::properties& tree) const;
    void           update();
    unsigned int   write(misc::shared_ptr<io::data> const& d);

  private:
    bool          _process_out;

    // Database parameters
    std::string   _metric_naming;
    std::string   _status_naming;
    std::string   _db_user;
    std::string   _db_password;
    std::string   _db_host;
    unsigned short _db_port;
    unsigned int  _queries_per_transaction;

    // Internal working members
    unsigned int _actual_query;

    // Status members
    std::string    _status;
    mutable QMutex _statusm;

    // Query
    query          _metric_query;
    query          _status_query;
    std::string    _query;

    void           _process_metric(storage::metric const& me);
    void           _process_status(storage::status const& st);
    void           _commit();
  };
}

CCB_END()

#endif // !CCB_GRAPHITE_STREAM_HH
