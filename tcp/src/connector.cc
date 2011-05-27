/*
** Copyright 2011 Merethis
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

#include "exceptions/basic.hh"
#include "tcp/connector.hh"
#include "tcp/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] c Object to copy.
 */
void connector::_internal_copy(connector const& c) {
  _host = c._host;
  _port = c._port;
  _socket = c._socket;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() : _port(0), _socket(new QTcpSocket) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c) : io::connector(c) {
  _internal_copy(c);
}

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
  io::connector::operator=(c);
  _internal_copy(c);
  return (*this);
}

/**
 *  Close the connector socket.
 */
void connector::close() {
  _socket->close();
  return ;
}

/**
 *  @brief Connect from another stream.
 *
 *  This method should not be called, as TCP directly handles
 *  connection.
 *
 *  @param[in] ptr Unused.
 */
void connector::connect(QSharedPointer<io::stream> ptr) {
  (void)ptr;
  throw (exceptions::basic() << "attempt to connect a TCP connector from another stream (this is a software bug)");
  return ;
}

/**
 *  Set connection parameters.
 *
 *  @param[in] host Host to connect to.
 *  @param[in] port Port to connect to.
 */
void connector::connect_to(QString const& host, unsigned short port) {
  _host = host;
  _port = port;
  return ;
}

/**
 *  Connect to the remote host.
 */
void connector::open() {
  // Launch connection process.
  _socket->connectToHost(_host, _port);

  // Wait for connection result.
  if (!_socket->waitForConnected())
    throw (exceptions::basic() << "could not connect to "
             << _host.toStdString().c_str() << ":" << _port
             << ": " << _socket->errorString().toStdString().c_str());

  // Forward stream.
  if (!_down.isNull()) {
    QSharedPointer<io::stream> ptr(new stream(_socket));
    _down->connect(ptr);
  }

  return ;
}
