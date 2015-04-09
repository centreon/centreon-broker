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

#include <sys/socket.h>
#include <QMutexLocker>
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
    _write_timeout(-1) {}

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
  logging::info(logging::medium) << "TCP: successfully connected to "
    << _host << ":" << _port;

  // Set the SO_KEEPALIVE option.
  _socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

  // Set the write timeout option.
  if (_write_timeout >= 0) {
#ifndef _WIN32
    struct timeval t;
    t.tv_sec = _write_timeout / 1000000;
    t.tv_usec = _write_timeout % 1000000;
    setsockopt(
      _socket->socketDescriptor(),
      SOL_SOCKET,
      SO_SNDTIMEO,
      &t,
      sizeof(t));
#endif //!_WIN32
  }

  // Return stream.
  misc::shared_ptr<stream> s(new stream(_socket, _mutex));
  s->set_timeout(_timeout);
  return (s);
}

/**
 *  Connect to the remote host.
 *
 *  @param[in] id     Unused.
 *  @param[in] cache  TCP module does not use the persistent cache.
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
 *  Set write timeout on data.
 *
 *  @param[in] msecs  Timeout in ms.
 */
void connector::set_write_timeout(int msecs) {
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
 *  @param[in] c Object to copy.
 */
void connector::_internal_copy(connector const& c) {
  _host = c._host;
  _port = c._port;
  _socket = c._socket;
  _timeout = c._timeout;
  _write_timeout = c._write_timeout;
  return ;
}
