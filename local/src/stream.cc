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
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/local/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::local;

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
 *  @param[in] sock Local socket object.
 */
stream::stream(QSharedPointer<QLocalSocket> sock)
  : _process_in(true),
    _process_out(true),
    _socket(sock) {}

/**
 *  Destructor.
 */
stream::~stream() {
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
 *  @return Data packet.
 */
QSharedPointer<io::data> stream::read() {
  if (!_process_in)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "local stream is shutdown");
  _socket->waitForReadyRead(-1);
  char buffer[2048];
  qint64 rb(_socket->read(buffer, sizeof(buffer)));
  if (rb < 0)
    throw (exceptions::msg() << "local: socket read error: "
             << _socket->errorString());
  QSharedPointer<io::raw> data(new io::raw);
  data->append(buffer, rb);
  return (data.staticCast<io::data>());
}

/**
 *  Write data to the socket.
 *
 *  @param[in] d Data to write.
 */
void stream::write(QSharedPointer<io::data> d) {
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "local stream is shutdown");
  if (d->type() == "com::centreon::broker::io::raw") {
    QSharedPointer<io::raw> r(d.staticCast<io::raw>());
    qint64 wb(_socket->write(static_cast<char*>(r->QByteArray::data()),
                                                r->size()));
    if (wb < 0)
      throw (exceptions::msg() << "local: write error: "
               << _socket->errorString());
    _socket->waitForBytesWritten(-1);
  }
  return ;
}
