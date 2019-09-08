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

#include "com/centreon/broker/notification/connector.hh"
#include "com/centreon/broker/notification/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 */
connector::connector(std::shared_ptr<persistent_cache> cache)
    : io::endpoint(false), _cache(cache), _node_cache(_cache) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
    : io::endpoint(c),
      _check_replication(c._check_replication),
      _centreon_db(c._centreon_db),
      _host(c._host),
      _password(c._password),
      _port(c._port),
      _type(c._type),
      _user(c._user),
      _cache(c._cache),
      _node_cache(c._node_cache) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  if (this != &c) {
    io::endpoint::operator=(c);
    _check_replication = c._check_replication;
    _centreon_db = c._centreon_db;
    _host = c._host;
    _password = c._password;
    _port = c._port;
    _type = c._type;
    _user = c._user;
    _cache = c._cache;
    _node_cache = c._node_cache;
  }
  return (*this);
}

/**
 *  Set connection parameters.
 *
 *  @param[in] type                    Database type.
 *  @param[in] host                    Database host.
 *  @param[in] port                    Database port.
 *  @param[in] user                    User.
 *  @param[in] password                Password.
 *  @param[in] centreon_db             Database name.
 *  @param[in] check_replication       true to check replication status.
 */
void connector::connect_to(std::string const& type,
                           std::string const& host,
                           unsigned short port,
                           std::string const& user,
                           std::string const& password,
                           std::string const& centreon_db,
                           bool check_replication) {
  _check_replication = check_replication;
  _centreon_db = centreon_db;
  _host = host;
  _password = password;
  _port = port;
  _type = type;
  _user = user;
  return;
}

/**
 *  Connect to a notification cache DB and return a notification stream.
 *
 *  @return Notification stream object.
 */
std::shared_ptr<io::stream> connector::open() {
  return (std::shared_ptr<io::stream>(
      new stream(_type, _host, _port, _user, _password, _centreon_db,
                 _check_replication, _node_cache)));
}
