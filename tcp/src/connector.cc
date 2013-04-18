/*
** Copyright 2011-2013 Merethis
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

#include <QMutexLocker>
#if QT_VERSION >= 0x040300
#  include <QSslKey>
#  include <QSslSocket>
#endif // Qt >= 4.3.0
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
    _mutex(new QMutex),
    _port(0),
    _timeout(-1),
    _tls(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c) : io::endpoint(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
connector::~connector() {
  this->close();
}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  if (this != &c) {
    this->close();
    io::endpoint::operator=(c);
    _internal_copy(c);
  }
  return (*this);
}

/**
 *  Clone the connector.
 *
 *  @return This object.
 */
io::endpoint* connector::clone() const {
  return (new connector(*this));
}

/**
 *  Close the connector socket.
 */
void connector::close() {
  QMutexLocker lock(&*_mutex);
  if (!_socket.isNull()) {
    _socket->close();
    _socket.clear();
  }
  return ;
}

/**
 *  Set connection parameters.
 *
 *  @param[in] host Host to connect to.
 *  @param[in] port Port to connect to.
 */
void connector::connect_to(QString const& host, unsigned short port) {
  this->close();
  _host = host;
  _port = port;
  return ;
}

/**
 *  Connect to the remote host.
 */
misc::shared_ptr<io::stream> connector::open() {
  // Close previous connection.
  this->close();

  // Lock mutex.
  QMutexLocker lock(&*_mutex);

#if QT_VERSION >= 0x040300
  // Is TLS enabled ?
  if (_tls) {
    // Create socket object.
    misc::shared_ptr<QSslSocket> ssl_socket(new QSslSocket);
    _socket = ssl_socket.staticCast<QTcpSocket>();

    // Use only TLS protocol.
    ssl_socket->setProtocol(QSsl::TlsV1);

    // Set self certificates.
    if (!_private.isEmpty() && !_public.isEmpty()) {
      ssl_socket->setLocalCertificate(_public);
      if (ssl_socket->localCertificate().isNull()
          || !ssl_socket->localCertificate().isValid())
        throw (exceptions::msg()
               << "TCP: invalid public certificate file '"
               << qPrintable(_public) << "'");
      ssl_socket->setPrivateKey(_private);
      if (ssl_socket->privateKey().isNull())
        throw (exceptions::msg() << "TCP: invalid private key file '"
               << qPrintable(_private) << "'");
    }

    // Set CA certificate.
    if (!_ca.isEmpty()) {
      ssl_socket->addCaCertificates(_ca);
      ssl_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
      ssl_socket->setPeerVerifyDepth(0);
    }
    else
      ssl_socket->setPeerVerifyMode(QSslSocket::VerifyNone);

    // Launch connection and handshake process.
    ssl_socket->connectToHostEncrypted(_host, _port);

    // Wait for connection result.
    if (!ssl_socket->waitForEncrypted())
      throw (exceptions::msg() << "TCP: could not connect to "
               << _host << ":" << _port << ": "
               << _socket->errorString());
  }
  else
#endif // Qt >= 4.3.0
  {
    // Launch connection process.
    logging::info(logging::medium) << "TCP: connecting to "
      << _host << ":" << _port;
    _socket = misc::shared_ptr<QTcpSocket>(new QTcpSocket);
    _socket->connectToHost(_host, _port);

    // Wait for connection result.
    if (!_socket->waitForConnected())
      throw (exceptions::msg() << "TCP: could not connect to "
               << _host << ":" << _port << ": "
               << _socket->errorString());
  }
  logging::info(logging::medium) << "TCP: successfully connected to "
    << _host << ":" << _port;

  // Return stream.
  misc::shared_ptr<stream> s(new stream(_socket, _mutex));
  s->set_timeout(_timeout);
  return (s.staticCast<io::stream>());
}

/**
 *  Connect to the remote host.
 *
 *  @param[in] id Unused.
 */
misc::shared_ptr<io::stream> connector::open(QString const& id) {
  (void)id;
  return (open());
}

/**
 *  Set connection timeout.
 *
 *  @param[in] msecs Timeout in ms.
 */
void connector::set_timeout(int msecs) {
  _timeout = msecs;
  return ;
}

/**
 *  Set TLS parameters.
 *
 *  @param[in] enable      true to enable, false to disable.
 *  @param[in] private_key Private key to use for encryption.
 *  @param[in] public_cert Public certificate to use for encryption.
 *  @param[in] ca_cert     Trusted CA's certificate, used to
 *                         authenticate peers.
 */
void connector::set_tls(
                  bool enable,
                  QString const& private_key,
                  QString const& public_cert,
                  QString const& ca_cert) {
  _ca = ca_cert;
  _private = private_key;
  _public = public_cert;
  _tls = enable;
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
 *  @param[in] c Object to copy.
 */
void connector::_internal_copy(connector const& c) {
  _ca = c._ca;
  _host = c._host;
  _port = c._port;
  _private = c._private;
  _public = c._public;
  _socket = c._socket;
  _timeout = c._timeout;
  _tls = c._tls;
  return ;
}
