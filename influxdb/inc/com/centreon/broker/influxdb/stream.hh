/*
** Copyright 2011-2017 Centreon
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

#ifndef CCB_INFLUXDB_STREAM_HH
#define CCB_INFLUXDB_STREAM_HH

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include "com/centreon/broker/influxdb/column.hh"
#include "com/centreon/broker/influxdb/influxdb.hh"
#include "com/centreon/broker/influxdb/macro_cache.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

// Forward declaration.
class database_config;

namespace influxdb {
/**
 *  @class stream stream.hh "com/centreon/broker/influxdb/stream.hh"
 *  @brief Influxdb stream.
 *
 *  Insert metrics into influxdb.
 */
class stream : public io::stream {
 public:
  stream(std::string const& user,
         std::string const& passwd,
         std::string const& addr,
         unsigned short port,
         std::string const& db,
         unsigned int queries_per_transaction,
         std::string const& status_ts,
         std::vector<column> const& status_cols,
         std::string const& metric_ts,
         std::vector<column> const& metric_cols,
         std::shared_ptr<persistent_cache> const& cache);
  ~stream();
  int flush();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void statistics(json11::Json::object& tree) const;
  void update();
  int write(std::shared_ptr<io::data> const& d);

 private:
  // Database parameters
  std::string _user;
  std::string _password;
  std::string _address;
  std::string _db;
  unsigned int _queries_per_transaction;
  std::unique_ptr<influxdb> _influx_db;

  // Internal working members
  int _pending_queries;
  unsigned int _actual_query;
  bool _commit;

  // Cache
  macro_cache _cache;

  // Status members
  std::string _status;
  mutable std::mutex _statusm;
};
}  // namespace influxdb

CCB_END()

#endif  // !CCB_INFLUXDB_STREAM_HH
