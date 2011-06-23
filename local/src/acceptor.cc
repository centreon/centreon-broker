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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/local/acceptor.hh"
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
acceptor::acceptor() {}

/**
 *  @brief Copy constructor.
 *
 *  Copy connection parameters but does not put socket in listen mode.
 *  You'll still need to call open() after the construction.
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : io::endpoint(a) {
  _name = a._name;
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  this->close();
}

/**
 *  @brief Assignment operator.
 *
 *  Destroy the old listener and copy connection parameters. However the
 *  internal socket won't be in listen mode until you call open().
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  this->close();
  io::endpoint::operator=(a);
  _name = a._name;
  return (*this);
}

/**
 *  Close the socket.
 */
void acceptor::close() {
  if (!_socket.isNull()) {
    _socket->close();
    _socket.reset();
  }
  return ;
}

/**
 *  Set the local socket name.
 *
 *  @param[in] name Local socket name.
 */
void acceptor::listen_on(QString const& name) {
  _name = name;
  return ;
}

/**
 *  Open the acceptor.
 *
 *  @return A new stream.
 */
QSharedPointer<io::stream> acceptor::open() {
  // Listen.
  if (_socket.isNull()) {
    _socket.reset(new QLocalServer);
    if (!_socket->listen(_name)) {
      exceptions::msg e;
      e << "local: could not listen on socket '" << _name
        << "': " << _socket->errorString();
      _socket.reset();
      throw (e);
    }
  }

  // Wait for incoming connections.
  logging::debug << logging::MEDIUM
    << "local: waiting for new connection";
  if (!_socket->waitForNewConnection(-1))
    throw (exceptions::msg() << "local: error while waiting for new " \
             "client: " << _socket->errorString());

  // Accept client.
  QSharedPointer<QLocalSocket> incoming(_socket->nextPendingConnection());
  if (incoming.isNull())
    throw (exceptions::msg() << "local: could not accept incoming " \
             "client: " << _socket->errorString());
  logging::info << logging::MEDIUM
    << "local: new client successfully connected";

  // Return object.
  return (QSharedPointer<io::stream>(new stream(incoming)));
}
