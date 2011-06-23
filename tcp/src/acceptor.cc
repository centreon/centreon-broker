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

#include <QScopedPointer>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/stream.hh"
#include "com/centreon/broker/tcp/tls_server.hh"

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
 *  @param[in] a Object to copy.
 */
void acceptor::_internal_copy(acceptor const& a) {
  _ca = a._ca;
  _port = a._port;
  _private = a._private;
  _public = a._public;
  _tls = a._tls;
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
acceptor::acceptor() : _port(0), _tls(false) {}

/**
 *  @brief Copy constructor.
 *
 *  The constructor only copy connection parameters. The socket will
 *  have to be opened after construction using open().
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : io::endpoint(a) {
  _internal_copy(a);
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {}

/**
 *  @brief Assignment operator.
 *
 *  The method close the previous connection and copy the connection
 *  parameters of the object. The socket will have to be opened after
 *  assignment using open().
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  this->close();
  io::endpoint::operator=(a);
  _internal_copy(a);
  return (*this);
}

/**
 *  Close the acceptor.
 */
void acceptor::close() {
  if (!_socket.isNull()) {
    _socket->close();
    _socket.reset();
  }
  return ;
}

/**
 *  Set the port on which the acceptor will listen.
 *
 *  @param[in] port Port on which the acceptor will listen.
 */
void acceptor::listen_on(unsigned short port) {
  this->close();
  _port = port;
  return ;
}

/**
 *  Start connection acception.
 */
QSharedPointer<io::stream> acceptor::open() {
  // Listen on port.
  if (_socket.isNull()) {
    _socket.reset(_tls ? new tls_server(_private, _public, _ca)
                       : new QTcpServer);
    if (!_socket->listen(QHostAddress::Any, _port)) {
      exceptions::msg e;
      e << "TCP: could not listen on port " << _port
        << ": " << _socket->errorString();
      _socket.reset();
      throw (e);
    }
  }

  // Wait for incoming connections.
  logging::debug << logging::MEDIUM << "TCP: waiting for new connection";
  if (!_socket->waitForNewConnection(-1))
    throw (exceptions::msg() << "TCP: error while waiting for " \
             "client: " << _socket->errorString());

  // Accept client.
  QSharedPointer<QTcpSocket> incoming(_socket->nextPendingConnection());
  if (incoming.isNull())
    throw (exceptions::msg() << "TCP: could not accept client: "
             << _socket->errorString());
  logging::info << logging::MEDIUM << "TCP: new client connected";

  // Return object.
  return (QSharedPointer<io::stream>(new stream(incoming)));
}

/**
 *  Set TLS parameters.
 *
 *  @param[in] enable true to enable TLS encryption.
 *  @param[in] private_key Private key to use for encryption.
 *  @param[in] public_cert Public certificate to use for encryption.
 *  @param[in] ca_cert     Trusted CA's certificate, used to
 *                         authenticate peers.
 */
void acceptor::set_tls(bool enable,
                       QString const& private_key,
                       QString const& public_cert,
                       QString const& ca_cert) {
  _ca = ca_cert;
  _private = private_key;
  _public = public_cert;
  _tls = enable;
  return ;
}
