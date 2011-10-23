/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/local/connector.hh"
#include "com/centreon/broker/local/stream.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::local;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() : io::endpoint(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Only connection parameters are copied. The socket has to be opened
 *  after construction using open().
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c) : io::endpoint(c) {
  _name = c._name;
}

/**
 *  Destructor.
 */
connector::~connector() {
  this->close();
}

/**
 *  @brief Assignment operator.
 *
 *  The previous connection is first closed and then connection
 *  parameters are copied. The socket has to be opened after assignment
 *  using open().
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  this->close();
  io::endpoint::operator=(c);
  _name = c._name;
  return (*this);
}

/**
 *  Close the socket.
 */
void connector::close() {
  if (!_socket.isNull()) {
    _socket->disconnectFromServer();
    _socket->waitForDisconnected(-1);
    _socket.clear();
  }
  return ;
}

/**
 *  Set the connection parameters.
 *
 *  @param[in] name Socket name.
 */
void connector::connect_to(QString const& name) {
  this->close();
  _name = name;
  return ;
}

/**
 *  Open the connection.
 *
 *  @return A new connection object.
 */
QSharedPointer<io::stream> connector::open() {
  // Close previous connection.
  this->close();

  // Launch connection process.
  _socket = QSharedPointer<QLocalSocket>(new QLocalSocket);
  _socket->connectToServer(_name);

  // Wait for connection result.
  if (!_socket->waitForConnected(-1)) {
    exceptions::msg e;
    e << "local: could not connect to '" << _name
      << "': " << _socket->errorString();
    _socket.clear();
    throw (e);
  }
  logging::info << logging::MEDIUM
    << "local: successfully connected to '" << _name << "'";

  // Return stream.
  return (QSharedPointer<io::stream>(new stream(_socket)));
}
