/*
** Copyright 2011-2013,2017 Centreon
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

#ifndef CCB_GRAPHITE_CONNECTOR_HH
#define CCB_GRAPHITE_CONNECTOR_HH

#include <ctime>
#include <memory>
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace graphite {
/**
 *  @class connector connector.hh "com/centreon/broker/graphite/connector.hh"
 *  @brief Connect to a graphite stream.
 */
class connector : public io::endpoint {
  std::string _escape_string;
  std::string _metric_naming;
  std::string _status_naming;
  std::string _user;
  std::string _password;
  std::string _addr;
  unsigned short _port;
  uint32_t _queries_per_transaction;
  std::shared_ptr<persistent_cache> _persistent_cache;

 public:
  connector();
  ~connector() noexcept {}
  connector(const connector&) = delete;
  connector& operator=(const connector&) = delete;
  void connect_to(std::string const& metric_naming,
                  std::string const& status_naming,
                  std::string const& escape_string,
                  std::string const& db_user,
                  std::string const& db_passwd,
                  std::string const& db_host,
                  unsigned short db_port,
                  uint32_t queries_per_transaction,
                  std::shared_ptr<persistent_cache> const& cache);
  std::unique_ptr<io::stream> open() override;
};
}  // namespace graphite

CCB_END()

#endif  // !CCB_GRAPHITE_CONNECTOR_HH
