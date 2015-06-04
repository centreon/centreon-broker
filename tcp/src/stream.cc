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

#include <cstdlib>
#include <QMutexLocker>
#include <QWaitCondition>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
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
    _read_timeout(-1),
    _socket(sock),
    _write_timeout(-1) {}

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
    _read_timeout(-1),
    _socket(sock),
    _write_timeout(-1) {}

/**
 *  Destructor.
 */
stream::~stream() {
  QMutexLocker lock(&*_mutex);
  if (!_socket.isNull())
    _socket->close();
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
  d.clear();
  QMutexLocker lock(&*_mutex);

  // If data is already available, skip the waitForReadyRead() loop.
  if (_socket->bytesAvailable() <= 0) {
    while (1) {
      if ((deadline != (time_t)-1)
          && (time(NULL) >= deadline)) {
        return (false);
      }
      bool ret;
      if (!(ret = _socket->waitForReadyRead(200))
          // Disconnected socket with no data.
          && (_socket->state()
              == QAbstractSocket::UnconnectedState)
          && (_socket->bytesAvailable() <= 0))
        throw (exceptions::msg() << "TCP stream is disconnected");
      if (ret
          || (_socket->error() != QAbstractSocket::SocketTimeoutError)
          || (_socket->bytesAvailable() > 0))
        break ;
      else {
        QWaitCondition cv;
        cv.wait(&*_mutex, 1);
      }
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
  d = data;
  return (true);
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void stream::set_read_timeout(int secs) {
  _read_timeout = secs;
  return ;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Write timeout in seconds.
 */
void stream::set_write_timeout(int secs) {
  _write_timeout = secs;
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
  // Check that data exists and should be processed.
  if (d.isNull())
    return (1);

  if (d->type() == io::raw::static_type()) {
    misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
    logging::debug(logging::low) << "TCP: write request of "
      << r->size() << " bytes";
    QMutexLocker lock(&*_mutex);
    qint64 wb(_socket->write(static_cast<char*>(r->QByteArray::data()),
                             r->size()));
    if ((wb < 0) || (_socket->state() == QAbstractSocket::UnconnectedState))
      throw (exceptions::msg() << "TCP: error while writing: "
             << _socket->errorString());
    if (_socket->waitForBytesWritten(_write_timeout * 1000) == false)
      throw (exceptions::msg() << "TCP: error while sending data: "
             << _socket->errorString());
  }
  return (1);
}
