/*
** Copyright 2015-2017 Centreon
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

#ifndef CCB_INFLUXDB_INFLUXDB12_HH
#define CCB_INFLUXDB_INFLUXDB12_HH

#include <asio.hpp>
#include <memory>
#include <string>
#include "com/centreon/broker/influxdb/column.hh"
#include "com/centreon/broker/influxdb/influxdb.hh"
#include "com/centreon/broker/influxdb/line_protocol_query.hh"
#include "com/centreon/broker/influxdb/macro_cache.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/storage/metric.hh"

#if ASIO_VERSION < 101200
namespace asio {
typedef io_service io_context;
}
#endif

CCB_BEGIN()

namespace influxdb {
/**
 *  @class influxdb influxdb12.hh "com/centreon/broker/influxdb/influxdb12.hh"
 *  @brief Influxdb connection/query manager.
 *
 *  This object manage connection and query to influxdb through the Lina
 *  API.
 */
class influxdb12 : public influxdb::influxdb {
 public:
  influxdb12(std::string const& user,
             std::string const& passwd,
             std::string const& addr,
             unsigned short port,
             std::string const& db,
             std::string const& status_ts,
             std::vector<column> const& status_cols,
             std::string const& metric_ts,
             std::vector<column> const& metric_cols,
             macro_cache const& cache);
  ~influxdb12();

  void clear();
  void write(storage::metric const& m);
  void write(storage::status const& s);
  void commit();

 private:
  std::string _post_header;
  std::string _url;
  std::string _query;
  line_protocol_query _status_query;
  line_protocol_query _metric_query;

  std::unique_ptr<asio::ip::tcp::socket> _socket;
  asio::io_context _io_context;

  std::string _host;
  unsigned short _port;

  macro_cache const& _cache;

  void _connect_socket();
  bool _check_answer_string(std::string const& ans);
  void _create_queries(std::string const& user,
                       std::string const& passwd,
                       std::string const& db,
                       std::string const& status_ts,
                       std::vector<column> const& status_cols,
                       std::string const& metric_ts,
                       std::vector<column> const& metric_cols);

  influxdb12(influxdb12 const& f);
  influxdb12& operator=(influxdb12 const& f);
};
}  // namespace influxdb

CCB_END()

#endif  // !CCB_INFLUXDB_INFLUXDB12_HH
