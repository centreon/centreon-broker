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

#include <assert.h>
#include <QMutexLocker>
#include <QWaitCondition>
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/tcp/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  (void)s;
  assert(false);
  abort();
  return (*this);
}

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
stream::stream(QSharedPointer<QTcpSocket> sock)
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
stream::stream(QSharedPointer<QTcpSocket> sock,
               QSharedPointer<QMutex> mutex)
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
 *  @return Data read.
 */
QSharedPointer<io::data> stream::read() {
  QMutexLocker lock(&*_mutex);
  bool ret;
  do {
    QWaitCondition cv;
    cv.wait(&*_mutex, 10);
    if (!_process_in
        || (!(ret = _socket->waitForReadyRead(
                (_timeout == -1)
                ? 200
                : _timeout))
            && _socket->state() != QAbstractSocket::UnconnectedState))
      throw (io::exceptions::shutdown(!_process_in, !_process_out)
               << "TCP stream is shutdown");
  } while (!ret
           && _socket->error() == QAbstractSocket::SocketTimeoutError);

  char buffer[2048];
  qint64 rb(_socket->read(buffer, sizeof(buffer)));
  if (rb < 0)
    throw (exceptions::msg() << "TCP: error while reading: "
             << _socket->errorString());
  QSharedPointer<io::raw> data(new io::raw);
  data->append(buffer, rb);
  return (data.staticCast<io::data>());
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
void stream::write(QSharedPointer<io::data> d) {
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "TCP stream is shutdown");
  if (d->type() == "com::centreon::broker::io::raw") {
    QSharedPointer<io::raw> r(d.staticCast<io::raw>());
    QMutexLocker lock(&*_mutex);
    qint64 wb(_socket->write(static_cast<char*>(r->QByteArray::data()),
                             r->size()));
    if (wb < 0)
      throw (exceptions::msg() << "TCP: error while writing: "
               << _socket->errorString());
    _socket->waitForBytesWritten(-1);
  }
  return ;
}
