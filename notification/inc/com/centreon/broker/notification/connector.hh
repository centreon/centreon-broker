/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_SQL_CONNECTOR_HH
#define CCB_SQL_CONNECTOR_HH

#include <QString>
#include <memory>
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/node_cache.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class connector connector.hh
 * "com/centreon/broker/notification/connector.hh"
 *  @brief Connect to a database.
 *
 *  Send events to a notification cache database.
 */
class connector : public io::endpoint {
 public:
  connector(std::shared_ptr<persistent_cache> cache);
  connector(connector const& c);
  ~connector();
  connector& operator=(connector const& c);
  void connect_to(std::string const& type,
                  std::string const& host,
                  unsigned short port,
                  std::string const& user,
                  std::string const& password,
                  std::string const& centreon_db,
                  bool check_replication = true);
  std::shared_ptr<io::stream> open();

 private:
  bool _check_replication;
  std::string _centreon_db;
  std::string _host;
  std::string _password;
  unsigned short _port;
  std::string _type;
  std::string _user;
  std::shared_ptr<persistent_cache> _cache;
  node_cache _node_cache;
};
}  // namespace notification

CCB_END()

#endif  // !CCB_SQL_CONNECTOR_HH
