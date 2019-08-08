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

#ifndef CCB_GRAPHITE_STREAM_HH
#  define CCB_GRAPHITE_STREAM_HH

#  include <asio.hpp>
#  include <deque>
#  include <list>
#  include <map>
#  include <memory>
#  include <mutex>
#  include <QSqlDatabase>
#  include <QString>
#  include <utility>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/multiplexing/hooker.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/storage/metric.hh"
#  include "com/centreon/broker/storage/status.hh"
#  include "com/centreon/broker/graphite/query.hh"
#  include "com/centreon/broker/graphite/macro_cache.hh"

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
                     std::string const& escape_string,
                     std::string const& db_user,
                     std::string const& db_password,
                     std::string const& db_host,
                     unsigned short db_port,
                     unsigned int queries_per_transaction,
                     std::shared_ptr<persistent_cache> const& cache);
                   ~stream();
    int            flush();
    bool           read(std::shared_ptr<io::data>& d, time_t deadline);
    void           statistics(io::properties& tree) const;
    void           update();
    int            write(std::shared_ptr<io::data> const& d);

  private:
    // Database parameters
    std::string    _metric_naming;
    std::string    _status_naming;
    std::string    _db_user;
    std::string    _db_password;
    std::string    _db_host;
    unsigned short _db_port;
    unsigned int   _queries_per_transaction;

    // Internal working members
    int            _pending_queries;
    unsigned int   _actual_query;
    bool           _commit_flag;

    // Status members
    std::string    _status;
    mutable std::mutex
                   _statusm;

    // Cache
    macro_cache    _cache;

    // Query
    query          _metric_query;
    query          _status_query;
    std::string    _query;
    std::string    _auth_query;
    std::unique_ptr<asio::ip::tcp::socket>
                   _socket;
    asio::io_context
                   _io_context;

    // Process metric/status and generate query.
    bool           _process_metric(storage::metric const& me);
    bool           _process_status(storage::status const& st);
    void           _commit();
  };
}

CCB_END()

#endif // !CCB_GRAPHITE_STREAM_HH
