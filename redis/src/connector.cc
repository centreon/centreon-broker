/*
** Copyright 2018 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/redis/connector.hh"
#include "com/centreon/broker/redis/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

/**
 *  Default constructor.
 */
connector::connector()
  : io::endpoint(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other)
  : io::endpoint(other),
    _address(other._address),
    _port(other._port),
    _user(other._user),
    _password(other._password) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _address = other._address;
    _port = other._port;
    _user = other._user;
    _password = other._password;
  }
  return (*this);
}

/**
 *  Set connection parameters.
 *
 * @param[in] address  The Redis server ip address
 * @param[in] port     The Redis server port
 * @param[in] user     The user to connect to the server
 * @param[in] password The user's password
 */
void connector::connect_to(
                  std::string const& address,
                  unsigned short port,
                  std::string const& user,
                  std::string const& password) {
  _address = address;
  _port = port;
  _user = user;
  _password = password;
}

/**
 *  Connect to the redis connector.
 *
 *  @return a redis connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(new stream(
            _address,
            _port,
            _user,
            _password)));
}
