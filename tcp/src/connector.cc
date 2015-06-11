/*
** Copyright 2011-2015 Merethis
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
  return (*this);
}

/**
 *  Set connection parameters.
 *
 *  @param[in] host  Host to connect to.
 *  @param[in] port  Port to connect to.
 */
void connector::connect_to(QString const& host, unsigned short port) {
  _host = host;
  _port = port;
  return ;
}

/**
 *  Connect to the remote host.
 */
misc::shared_ptr<io::stream> connector::open() {
  // Launch connection process.
  logging::info(logging::medium) << "TCP: connecting to "
    << _host << ":" << _port;
  std::string connection_name;
  {
    std::ostringstream oss;
    oss << _host.toStdString() << ":" << _port;
    connection_name = oss.str();
  }
  std::auto_ptr<QTcpSocket> sock(new QTcpSocket);
  sock->connectToHost(_host, _port);

  // Wait for connection result.
  if (!sock->waitForConnected())
    throw (exceptions::msg() << "TCP: could not connect to "
           << connection_name << ": " << sock->errorString());
  logging::info(logging::medium)
    << "TCP: successfully connected to " << connection_name;

  // Return stream.
  misc::shared_ptr<stream> s(new stream(sock.get(), connection_name));
  sock.release();
  s->set_read_timeout(_read_timeout);
  s->set_write_timeout(_write_timeout);
  return (s);
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_read_timeout(int secs) {
  _read_timeout = secs;
  return ;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_write_timeout(int secs) {
  _write_timeout = secs;
  return ;
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
  return ;
}
