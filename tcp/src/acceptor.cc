/*
** Copyright 2011-2013,2015 Merethis
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

#include <sys/socket.h>
#include <QMutexLocker>
#include <QWaitCondition>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
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
acceptor::acceptor() : io::endpoint(true), _port(0), _write_timeout(-1) {}

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
 *  Clone the acceptor.
 *
 *  @return This object.
 */
io::endpoint* acceptor::clone() const {
  return (new acceptor(*this));
}

/**
 *  Close the acceptor.
 */
void acceptor::close() {
  QMutexLocker lock(&_mutex);
  if (_socket.get()) {
    {
      QMutexLocker childrenm(&_childrenm);
      while (!_children.isEmpty()) {
        QPair<misc::shared_ptr<QTcpSocket>, misc::shared_ptr<QMutex> >
          p(_children.front());
        _children.pop_front();
        childrenm.unlock();
        QMutexLocker l(p.second.data());
        p.first->close();
        childrenm.relock();
      }
    }
    _socket->close();
    _socket->deleteLater();
    _socket.release();
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
 *
 */
misc::shared_ptr<io::stream> acceptor::open() {
  // Listen on port.
  QMutexLocker lock(&_mutex);
  if (!_socket.get()) {
    _socket.reset(new QTcpServer);
    if (!_socket->listen(QHostAddress::Any, _port)) {
      exceptions::msg e;
      e << "TCP: could not listen on port " << _port
        << ": " << _socket->errorString();
      _socket.reset();
      throw (e);
    }
  }

  // Wait for incoming connections.
  if (!_socket->hasPendingConnections()) {
    logging::debug(logging::medium) << "TCP: waiting for new connection";
    bool timedout(false);
    bool ret(_socket->waitForNewConnection(200, &timedout));
    while (!ret && timedout) {
      QWaitCondition cv;
      cv.wait(&_mutex, 10);
      timedout = false;
      ret = _socket.get()
	&& _socket->waitForNewConnection(200, &timedout);
    }
    if (!ret) {
      if (!_socket.get())
	throw (io::exceptions::shutdown(true, true)
	       << "TCP: shutdown requested");
      else
	throw (exceptions::msg() << "TCP: error while waiting client: "
	       << _socket->errorString());
    }
  }

  // Accept client.
  misc::shared_ptr<QTcpSocket> incoming(_socket->nextPendingConnection());
  if (incoming.isNull())
    throw (exceptions::msg() << "TCP: could not accept client: "
             << _socket->errorString());
  logging::info(logging::medium) << "TCP: new client connected";

  // Set the SO_KEEPALIVE option.
  incoming->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

  // Set the write timeout option.
  if (_write_timeout >= 0) {
#ifndef _WIN32
    struct timeval t;
    t.tv_sec = _write_timeout % 1000000;
    t.tv_usec = _write_timeout / 1000000;
    setsockopt(
      incoming->socketDescriptor(),
      SOL_SOCKET,
      SO_SNDTIMEO,
      &t,
      sizeof(t));
#endif //!_WIN32
  }

  // Create child objects.
  misc::shared_ptr<QMutex> mutex(new QMutex);
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
    QPair<misc::shared_ptr<QTcpSocket>, misc::shared_ptr<QMutex> >
      tmp(incoming, mutex);
    _children.push_back(tmp);
  }

  // Return object.
  return (misc::shared_ptr<io::stream>(new stream(incoming, mutex)));
}

/**
 *  Start connection acception.
 *
 *  @param[in] id     Unused.
 */
misc::shared_ptr<io::stream> acceptor::open(QString const& id) {
  (void)id;
  return (open());
}

/**
 *  Get statistics about this TCP acceptor.
 *
 *  @param[out] tree Buffer in which statistics will be written.
 */
void acceptor::stats(io::properties& tree) {
  QMutexLocker children_lock(&_childrenm);
  std::ostringstream oss;
  oss << "peers=" << _children.size();
  for (QList<QPair<misc::shared_ptr<QTcpSocket>, misc::shared_ptr<QMutex> > >::iterator
         it(_children.begin()),
         end(_children.end());
       it != end;
       ++it) {
    QMutexLocker lock(it->second.data());
    if (!it->first.isNull())
      oss << "\n  " << it->first->peerAddress().toString().toStdString()
          << ":" << it->first->peerPort();
  }
  io::property& p(tree["peers"]);
  p.set_perfdata(oss.str());
  p.set_graphable(false);
  return ;
}

/**
 *  Set write timeout on data.
 *
 *  @param[in] msecs  Timeout in ms.
 */
void acceptor::set_write_timeout(int msecs) {
  _write_timeout = msecs;
}

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
  _port = a._port;
  _write_timeout = a._write_timeout;
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
  QTcpSocket* sock(static_cast<QTcpSocket*>(obj));

  // Find and remove matching entry.
  QMutexLocker lock(&_childrenm);
  for (QList<QPair<misc::shared_ptr<QTcpSocket>, misc::shared_ptr<QMutex> > >::iterator
         it(_children.begin()),
         end(_children.end());
       it != end;
         ++it)
    if (it->first.data() == sock) {
      {
        QPair<misc::shared_ptr<QTcpSocket>, misc::shared_ptr<QMutex> >
          p(*it);
        _children.erase(it);
        lock.unlock();
      } // Eventual shared_ptr destruction, p going out of scope.
      lock.relock();
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
  if (e != QAbstractSocket::SocketTimeoutError)
    _on_stream_destroy(QObject::sender());
  return ;
}
