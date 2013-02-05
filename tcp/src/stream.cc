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

#include <cstdlib>
#include <QMutexLocker>
#include <QWaitCondition>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
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
 *  @param[in] sock Socket used by this stream.
 */
stream::stream(misc::shared_ptr<QTcpSocket> sock)
  : _mutex(new QMutex),
    _process_in(true),
    _process_out(true),
    _socket(sock),
    _timeout(-1) {}

/**
 *  Constructor.
 *
 *  @param[in] sock  Socket used by this stream.
 *  @param[in] mutex Mutex used by this stream.
 */
stream::stream(
          misc::shared_ptr<QTcpSocket> sock,
          misc::shared_ptr<QMutex> mutex)
  : _mutex(mutex),
    _process_in(true),
    _process_out(true),
    _socket(sock),
    _timeout(-1) {}

/**
 *  Destructor.
 */
stream::~stream() {
  QMutexLocker lock(&*_mutex);
  if (!_socket.isNull())
    _socket->close();
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
 *  Read data from the socket.
 *
 *  @param[out] d Data read.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  QMutexLocker lock(&*_mutex);

  // Check processing flags.
  if (!_process_in)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "TCP stream is shutdown");

  // If data is already available, skip the waitForReadyRead() loop.
  if (_socket->bytesAvailable() <= 0) {
    bool ret;
    while (1) {
      if (!(ret = _socket->waitForReadyRead(
                             (_timeout == -1)
                             ? 200
                             : _timeout))
          // Standalone socket.
          && ((_timeout != -1)
              // Disconnected socket with no data.
              || ((_socket->state()
                   == QAbstractSocket::UnconnectedState)
                  && (_socket->bytesAvailable() <= 0))))
        throw (exceptions::msg() << "TCP stream is disconnected");
      if (ret
          || (_socket->error() != QAbstractSocket::SocketTimeoutError)
          || (_socket->bytesAvailable() > 0))
        break ;
      else {
        QWaitCondition cv;
        cv.wait(&*_mutex, 1);
      }
      if (!_process_in)
        throw (io::exceptions::shutdown(!_process_in, !_process_out)
               << "TCP stream is shutdown");
    }
  }

  char buffer[2048];
  qint64 rb(_socket->read(buffer, sizeof(buffer)));
  if (rb < 0)
    throw (exceptions::msg() << "TCP: error while reading: "
           << _socket->errorString());
  misc::shared_ptr<io::raw> data(new io::raw);
#if QT_VERSION >= 0x040500
  data->append(buffer, rb);
#else
  data->append(QByteArray(buffer, rb));
#endif // Qt version
  d = data.staticCast<io::data>();
  return ;
}

/**
 *  Set connection timeout.
 *
 *  @param[in] msecs Timeout in ms.
 */
void stream::set_timeout(int msecs) {
  _timeout = msecs;
  return ;
}

/**
 *  Write data to the socket.
 *
 *  @param[in] d Data to write.
 */
void stream::write(misc::shared_ptr<io::data> const& d) {
  // Raw type.
  static QString const raw_type("com::centreon::broker::io::raw");

  // Check that data exists and should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "TCP stream is shutdown");
  if (d.isNull())
    return ;

  if (d->type() == raw_type) {
    misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
    logging::debug(logging::low) << "TCP: write request of "
      << r->size() << " bytes";
    QMutexLocker lock(&*_mutex);
    qint64 wb(_socket->write(static_cast<char*>(r->QByteArray::data()),
                             r->size()));
    if ((wb < 0) || (_socket->state() == QAbstractSocket::UnconnectedState))
      throw (exceptions::msg() << "TCP: error while writing: "
             << _socket->errorString());
    _socket->waitForBytesWritten(-1);
  }
  return ;
}
