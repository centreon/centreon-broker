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

#include <QWaitCondition>
#include <sstream>
#include <sys/socket.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
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
 *  Constructor.
 *
 *  @param[in] sock  Socket used by this stream.
 *  @param[in] name  Name of this connection.
 */
stream::stream(QTcpSocket* sock, std::string const& name)
  : _name(name),
    _parent(NULL),
    _read_timeout(-1),
    _socket(sock),
    _socket_descriptor(-1),
    _write_timeout(-1) {
  _set_socket_options();
}

/**
 *  Constructor.
 *
 *  @param[in] socket_descriptor  Native socket descriptor.
 */
stream::stream(int socket_descriptor)
  : _parent(NULL),
    _read_timeout(-1),
    _socket_descriptor(socket_descriptor),
    _write_timeout(-1) {}

/**
 *  Destructor.
 */
stream::~stream() {
  // Destructor of socket will properly shutdown connection.
  if (_socket_description != -1)
    _initialize_socket();
  // Close the socket.
  if (_socket.get())
    _socket->close();
  // Remove from parent.
  if (_parent)
    _parent->remove_child(_name);
}

/**
 *  Read data with timeout.
 *
 *  @param[out] d         Received event if any.
 *  @param[in]  deadline  Timeout in seconds.
 *
 *  @return Respects io::stream::read()'s return value.
 */
bool stream::read(
               misc::shared_ptr<io::data>& d,
               time_t deadline) {
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Set deadline.
  {
    time_t now = ::time(NULL);
    if (_read_timeout != -1
        && (deadline == (time_t)-1
            || now + _read_timeout < deadline))
      deadline = now + _read_timeout / 1000;
  }

  // If data is already available, skip the waitForReadyRead() loop.
  d.clear();
  if (_socket->bytesAvailable() <= 0) {
    bool ret(_socket->waitForReadyRead(0));
    while (_socket->bytesAvailable() <= 0) {
      // Request timeout.
      if ((deadline != (time_t)-1)
          && (time(NULL) >= deadline))
        return (false);
      // Disconnected socket with no data.
      else if (!ret
          && (_socket->state() == QAbstractSocket::UnconnectedState)
          && (_socket->bytesAvailable() <= 0))
        throw (exceptions::msg() << "TCP peer '"
               << _name << "' is disconnected");
      // Got data.
      else if (ret
          || (_socket->error() != QAbstractSocket::SocketTimeoutError)
          || (_socket->bytesAvailable() > 0))
        break ;

      // Wait for data.
      _socket->waitForReadyRead(200);
    }
  }

  char buffer[2048];
  qint64 rb(_socket->read(buffer, sizeof(buffer)));
  if (rb < 0)
    throw (exceptions::msg()
           << "error while reading from TCP peer '"
           << _name << "': " << _socket->errorString());
  misc::shared_ptr<io::raw> data(new io::raw);
#if QT_VERSION >= 0x040500
  data->append(buffer, rb);
#else
  data->append(QByteArray(buffer, rb));
#endif // Qt version
  d = data;
  return (true);
}

/**
 *  Set parent socket.
 *
 *  @param[in,out] parent  Parent socket.
 */
void stream::set_parent(acceptor* parent) {
  _parent = parent;
  return ;
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void stream::set_read_timeout(int secs) {
  if (secs == -1)
    _read_timeout = -1;
  else
    _read_timeout = secs * 1000;
  return ;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Write timeout in seconds.
 */
void stream::set_write_timeout(int secs) {
  if (secs == -1)
    _write_timeout = -1;
  else
    _write_timeout = secs * 1000;
  return ;
}

/**
 *  Write data to the socket.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Check that data exists and should be processed.
  if (!validate(d, "TCP"))
    return (1);

  if (d->type() == io::raw::static_type()) {
    misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
    logging::debug(logging::low) << "TCP: write request of "
      << r->size() << " bytes to peer '" << _name << "'";
    qint64 wb(_socket->write(static_cast<char*>(r->QByteArray::data()),
                             r->size()));
    if ((wb < 0) || (_socket->state() == QAbstractSocket::UnconnectedState))
      throw (exceptions::msg() << "TCP: error while writing to peer '"
             << _name << "': " << _socket->errorString());
    if (_socket->waitForBytesWritten(_write_timeout) == false)
      throw (exceptions::msg()
             << "TCP: error while sending data to peer '" << _name
             << "': " << _socket->errorString());
  }
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Initialize socket if it was not already initialized.
 */
void stream::_initialize_socket() {
  _socket.reset(new QTcpSocket);
  _socket->setSocketDescriptor(_socket_descriptor);
  _socket_descriptor = -1;
  {
    std::ostringstream oss;
    oss << _socket->peerAddress().toString().toStdString()
        << ":" << _socket->peerPort();
    _name = oss.str();
  }
  if (_parent)
    _parent->add_child(_name);
  _set_socket_options();
  return ;
}

/**
 *  Set various socket options.
 */
void stream::_set_socket_options() {
  // Set the SO_KEEPALIVE option.
  _socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

  // Set the write timeout option.
  if (_write_timeout >= 0) {
#ifndef _WIN32
    struct timeval t;
    t.tv_sec = _write_timeout / 1000;
    t.tv_usec = _write_timeout % 1000;
    ::setsockopt(
        _socket->socketDescriptor(),
        SOL_SOCKET,
        SO_SNDTIMEO,
        &t,
        sizeof(t));
#endif //!_WIN32
  }
  return ;
}
