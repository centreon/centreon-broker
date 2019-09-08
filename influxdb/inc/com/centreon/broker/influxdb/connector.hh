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

#ifndef CCB_INFLUXDB_CONNECTOR_HH
#define CCB_INFLUXDB_CONNECTOR_HH

#include <ctime>
#include <memory>
#include <vector>
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/influxdb/column.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace influxdb {
/**
 *  @class connector connector.hh "com/centreon/broker/influxdb/connector.hh"
 *  @brief Connect to an influxdb stream.
 */
class connector : public io::endpoint {
 public:
  connector();
  connector(connector const& other);
  ~connector();
  connector& operator=(connector const& other);
  void connect_to(std::string const& user,
                  std::string const& passwd,
                  std::string const& addr,
                  unsigned short _port,
                  std::string const& db,
                  unsigned int queries_per_transaction,
                  std::string const& status_ts,
                  std::vector<column> const& status_cols,
                  std::string const& metric_ts,
                  std::vector<column> const& metric_cols,
                  std::shared_ptr<persistent_cache> const& cache);
  std::shared_ptr<io::stream> open();

 private:
  std::string _user;
  std::string _password;
  std::string _addr;
  unsigned short _port;
  std::string _db;
  unsigned int _queries_per_transaction;
  std::string _status_ts;
  std::vector<column> _status_cols;
  std::string _metric_ts;
  std::vector<column> _metric_cols;
  std::shared_ptr<persistent_cache> _cache;

  void _internal_copy(connector const& other);
};
}  // namespace influxdb

CCB_END()

#endif  // !CCB_INFLUXDB_CONNECTOR_HH
