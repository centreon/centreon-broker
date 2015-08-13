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

#include <QMutexLocker>
#include <QWaitCondition>
#include <sstream>
#include <sys/socket.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
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
  : _process_in(true),
    _process_out(true),
    _mutex(QMutex::Recursive),
    _name(name),
    _parent(NULL),
    _read_timeout(3),
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
  : _process_in(true),
    _process_out(true),
    _mutex(QMutex::Recursive),
    _parent(NULL),
    _read_timeout(3),
    _socket_descriptor(socket_descriptor),
    _write_timeout(-1) {}

/**
 *  Destructor.
 */
stream::~stream() {
  QMutexLocker lock(&_mutex);
  if (_socket_descriptor != -1) {
    _initialize_socket();
  }
  if (_socket.get()) {
    _socket->close();
    if (_parent)
      _parent->remove_child(*this);
  }
}

/**
 *  Enable or disable event processing.
 *
 *  @param[in] in  Set to true to enable input event processing.
 *  @param[in] out Set to true to enable output event processing.
 */
void stream::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
  return ;
}

/**
 *  Read data with timeout.
 *
 *  @param[out] d         Received event if any.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  QMutexLocker lock(&_mutex);
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Stopping asked.
  if (!_process_in && _socket.get() && _socket->isOpen())
      _socket->close();

  // If data is already available, skip the waitForReadyRead() loop.
  d.clear();
  time_t deadline = ::time(NULL) + _read_timeout;
  if (_socket->bytesAvailable() <= 0) {
    bool ret(_socket->waitForReadyRead(0));
    while (_socket->bytesAvailable() <= 0) {
      // Check process
      if (!_process_in) {
        _stop();
        throw (io::exceptions::shutdown(!_process_in, !_process_out)
                 << "TCP stream is shutdown");
      }
      // Request timeout.
      else if ((_read_timeout != (time_t)-1)
          && (time(NULL) >= deadline)) {
        return ;
      }
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
  return ;
}

/**
 *  Get the name of this stream.
 *
 *  @return  The name of this stream.
 */
std::string const& stream::get_name() const throw() {
  return (_name);
}

/**
 *  Set parent socket.
 *
 *  @param[in,out] parent  Parent socket.
 */
void stream::set_parent(socket_parent* parent) {
  _parent = parent;
  if (_parent)
    _parent->add_child(*this);
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
    _read_timeout = secs;
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
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  QMutexLocker lock(&_mutex);
  if (!_process_out) {
    _stop();
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "TCP stream is shutdown");
  }

  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Check that data exists and should be processed.
  if (d.isNull())
    return (1);

  if (d->type() == io::events::data_type<io::events::internal, 1>::value) {
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

/**
 *  @brief Close the stream.
 *
 *  This will only signal the stream that it should stop processing.
 */
void stream::close() {
  process(false, false);
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

/**
 *  Stop this stream.
 */
void stream::_stop() {
  if (_socket.get())
    _socket->close();
  if (_parent)
    _parent->remove_child(*this);
}
