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

#include <QMutexLocker>
#include <QScopedPointer>
#include <QWaitCondition>
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

/**
 *  Called when a child TCP socket is destroyed.
 */
void acceptor::_on_stream_destroy(QObject* obj) {
  // No object, no processing.
  if (!obj)
    return ;

  // Retrieve QTcpSocket object.
  QTcpSocket* sock(reinterpret_cast<QTcpSocket*>(obj));

  // Find and remove matching entry.
  QMutexLocker lock(&_childrenm);
  for (QList<QPair<QWeakPointer<QTcpSocket>, QSharedPointer<QMutex> > >::iterator
         it(_children.begin()),
         end(_children.end());
       it != end;
       ++it)
    if (it->first.data() == sock) {
      _children.erase(it);
      break ;
    }
  return ;
}

/**
 *  Called when a child TCP socket is disconnected.
 */
void acceptor::_on_stream_disconnected() {
  _on_stream_destroy(QObject::sender());
  return ;
}

/**
 *  Called when a child TCP socket has an error.
 */
void acceptor::_on_stream_error(QAbstractSocket::SocketError e) {
  (void)e;
  _on_stream_destroy(QObject::sender());
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
acceptor::acceptor() : io::endpoint(true), _port(0), _tls(false) {}

/**
 *  @brief Copy constructor.
 *
 *  The constructor only copy connection parameters. The socket will
 *  have to be opened after construction using open().
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : QObject(), io::endpoint(a) {
  _internal_copy(a);
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
 *  The method close the previous connection and copy the connection
 *  parameters of the object. The socket will have to be opened after
 *  assignment using open().
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  if (this != &a) {
    this->close();
    io::endpoint::operator=(a);
    _internal_copy(a);
  }
  return (*this);
}

/**
 *  Close the acceptor.
 */
void acceptor::close() {
  QMutexLocker lock(&_mutex);
  if (!_socket.isNull()) {
    {
      QMutexLocker childrenm(&_childrenm);
      for (QList<QPair<QWeakPointer<QTcpSocket>, QSharedPointer<QMutex> > >::iterator
             it = _children.begin(),
             end = _children.end();
           it != end;
           ++it) {
        QMutexLocker l(it->second.data());
        QSharedPointer<QTcpSocket> sock = it->first.toStrongRef();
        if (sock)
          sock->close();
      }
    }
    _socket->close();
    _socket->deleteLater();
    _socket.take();
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
  QMutexLocker lock(&_mutex);
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
  logging::debug(logging::medium) << "TCP: waiting for new connection";
  bool timedout(false);
  bool ret(_socket->waitForNewConnection(200, &timedout));
  while (!ret && timedout) {
    QWaitCondition cv;
    cv.wait(&_mutex, 10);
    timedout = false;
    ret = !_socket.isNull()
      && _socket->waitForNewConnection(200, &timedout);
  }
  if (!ret)
    throw (exceptions::msg() << "TCP: error while waiting client: "
             << (_socket.isNull()
                 ? "socket was deleted"
                 : _socket->errorString()));

  // Accept client.
  QSharedPointer<QTcpSocket> incoming(_socket->nextPendingConnection());
  if (incoming.isNull())
    throw (exceptions::msg() << "TCP: could not accept client: "
             << _socket->errorString());
  logging::info(logging::medium) << "TCP: new client connected";

  // Create child objects.
  QSharedPointer<QMutex> mutex(new QMutex);
  connect(
    incoming.data(),
    SIGNAL(destroyed(QObject*)),
    this,
    SLOT(_on_stream_destroy(QObject*)));
  connect(
    incoming.data(),
    SIGNAL(disconnected()),
    this,
    SLOT(_on_stream_disconnected()));
  connect(
    incoming.data(),
    SIGNAL(error(QAbstractSocket::SocketError)),
    this,
    SLOT(_on_stream_error(QAbstractSocket::SocketError)));
  {
    QMutexLocker lock(&_childrenm);
    QPair<QWeakPointer<QTcpSocket>, QSharedPointer<QMutex> > tmp(incoming, mutex);
    _children.push_back(tmp);
  }

  // Return object.
  return (QSharedPointer<io::stream>(new stream(incoming, mutex)));
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
