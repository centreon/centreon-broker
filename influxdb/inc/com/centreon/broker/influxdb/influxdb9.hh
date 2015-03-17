/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_INFLUXDB_INFLUXDB9_HH
#  define CCB_INFLUXDB_INFLUXDB9_HH

#  include <string>
#  include <memory>
#  include <QTcpSocket>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/influxdb/influxdb.hh"
#  include "com/centreon/broker/influxdb/column.hh"
#  include "com/centreon/broker/influxdb/query.hh"

CCB_BEGIN()

namespace         influxdb {
  /**
   *  @class influxdb influxdb.hh "com/centreon/broker/influxdb/influxdb.hh"
   *  @brief Influxdb connection/query manager.
   *
   *  This object manage connection and query to influxdb through the POST
   *  API.
   */
  class           influxdb9 : public influxdb::influxdb {
  public:
                  influxdb9(
                    std::string const& user,
                    std::string const& passwd,
                    std::string const& addr,
                    unsigned short port,
                    std::string const& db,
                    std::string const& status_ts,
                    std::vector<column> const& status_cols,
                    std::string const& metric_ts,
                    std::vector<column> const& metric_cols);
                  influxdb9(influxdb9 const& f);
                  ~influxdb9();
    influxdb9&    operator=(influxdb9 const& f);

    void          clear();
    void          write(storage::metric const& m);
    void          write(storage::status const& s);
    void          commit();

  private:
    std::string   _post_header;
    std::string   _db_header;
    std::string   _url;
    std::string   _query;
    query         _status_query;
    query         _metric_query;

    std::auto_ptr<QTcpSocket>
                  _socket;

    std::string   _host;
    unsigned short
                  _port;

    void          _connect_socket();
    bool          _check_answer_string(std::string const& ans);
    void          _create_queries(
                    std::string const& user,
                    std::string const& passwd,
                    std::string const& db,
                    std::string const& status_ts,
                    std::vector<column> const& status_cols,
                    std::string const& metric_ts,
                    std::vector<column> const& metric_cols);
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_INFLUXDB9_HH
