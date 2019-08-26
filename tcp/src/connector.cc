/*
** Copyright 2011-2015 Centreon
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

#include <memory>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector()
  : io::endpoint(false),
    _port(0),
    _read_timeout(-1),
    _write_timeout(-1) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other) : io::endpoint(other) {
  _internal_copy(other);
}

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
    _internal_copy(other);
  }
  return *this;
}

/**
 *  Set connection parameters.
 *
 *  @param[in] host  Host to connect to.
 *  @param[in] port  Port to connect to.
 */
void connector::connect_to(std::string const& host, unsigned short port) {
  _host = host;
  _port = port;
}

/**
 *  Connect to the remote host.
 */
std::shared_ptr<io::stream> connector::open() {
  // Launch connection process.
  logging::info(logging::medium) << "TCP: connecting to "
    << _host << ":" << _port;
  std::string connection_name;
  {
    std::ostringstream oss;
    oss << _host << ":" << _port;
    connection_name = oss.str();
  }
  std::unique_ptr<QTcpSocket> sock(new QTcpSocket);
  sock->connectToHost(QString::fromStdString(_host), _port);

  // Wait for connection result.
  if (!sock->waitForConnected())
    throw (exceptions::msg() << "TCP: could not connect to "
           << connection_name << ": " << sock->errorString());
  logging::info(logging::medium)
    << "TCP: successfully connected to " << connection_name;

  // Return stream.
  std::shared_ptr<stream> s(new stream(sock.get(), connection_name));
  sock.release();
  s->set_read_timeout(_read_timeout);
  s->set_write_timeout(_write_timeout);
  return s;
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_read_timeout(int secs) {
  _read_timeout = secs;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_write_timeout(int secs) {
  _write_timeout = secs;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void connector::_internal_copy(connector const& other) {
  _host = other._host;
  _port = other._port;
  _read_timeout = other._read_timeout;
  _write_timeout = other._write_timeout;
}
