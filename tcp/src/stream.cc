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

#include "exceptions/basic.hh"
#include "tcp/stream.hh"

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
stream::stream(stream const& s) : io::stream(s), _socket(s._socket) {}

/**
 *  Destructor.
 */
stream::~stream() {}

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
 *  @param[out] data Buffer in which to read.
 *  @param[in]  size Maximum size that can be read.
 *
 *  @return Number of bytes read.
 */
unsigned int stream::read(void* data, unsigned int size) {
  _socket->waitForReadyRead(-1);
  qint64 rb(_socket->read(static_cast<char*>(data), size));
  if (rb < 0)
    throw (exceptions::basic() << "TCP read error: "
             << _socket->errorString().toStdString().c_str());
  return (rb);
}

/**
 *  Write data to the socket.
 *
 *  @param[in] data Buffer to send.
 *  @param[in] size Maximum number of bytes to send.
 *
 *  @return Number of bytes written.
 */
unsigned int stream::write(void const* data, unsigned int size) {
  qint64 wb(_socket->write(static_cast<char const*>(data), size));
  if (wb < 0)
    throw (exceptions::basic() << "TCP write error: "
             << _socket->errorString().toStdString().c_str());
  return (wb);
}
