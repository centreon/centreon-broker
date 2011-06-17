/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
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
stream::stream(QSharedPointer<QTcpSocket> sock) : _socket(sock) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s)
  : io::stream(s), _socket(s._socket) {}

/**
 *  Destructor.
 */
stream::~stream() {
  if (!_socket.isNull())
    _socket->close();
}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  io::stream::operator=(s);
  _socket = s._socket;
  return (*this);
}

/**
 *  Read data from the socket.
 *
 *  @return Data read.
 */
QSharedPointer<io::data> stream::read() {
  _socket->waitForReadyRead(-1);
  char buffer[2048];
  qint64 rb(_socket->read(buffer, sizeof(buffer)));
  if (rb < 0)
    throw (exceptions::msg() << "TCP read error: "
             << _socket->errorString().toStdString().c_str());
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
  if (d->type() == "com::centreon::broker::io::raw") {
    QSharedPointer<io::raw> r(d.staticCast<io::raw>());
    qint64 wb(_socket->write(static_cast<char*>(r->memory()), r->size()));
    if (wb < 0)
      throw (exceptions::msg() << "TCP write error: "
               << _socket->errorString().toStdString().c_str());
    _socket->waitForBytesWritten(-1);
  }
  return ;
}
