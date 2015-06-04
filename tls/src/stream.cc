/*
** Copyright 2009-2014 Merethis
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

#include <cstdio>
#include <cstdlib>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/tls/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  When building the stream, you need to provide the session that will
 *  be used to transport encrypted data.
 *
 *  @param[in] sess  TLS session, providing informations on the
 *                   encryption that should be used.
 */
stream::stream(gnutls_session_t* sess) : _session(sess) {}

/**
 *  @brief Destructor.
 *
 *  The destructor will release all acquired ressources that haven't
 *  been released yet.
 */
stream::~stream() {
  if (_session) {
    gnutls_bye(*_session, GNUTLS_SHUT_RDWR);
    gnutls_deinit(*_session);
    delete (_session);
    _session = NULL;
  }
}

/**
 *  @brief Receive data from the TLS session.
 *
 *  Receive at most size bytes from the network stream and store them in
 *  buffer. The number of bytes read is then returned. This number can
 *  be less than size.
 *
 *  @param[out] d         Object that will be returned containing a
 *                        chunk of data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  // Clear existing content.
  d.clear();

  // Read data.
  misc::shared_ptr<io::raw> buffer(new io::raw);
  buffer->resize(BUFSIZ);
  int ret(gnutls_record_recv(
            *_session,
            buffer->QByteArray::data(),
            buffer->size()));
  if (!ret)
    throw (io::exceptions::shutdown(true, false)
           << "TLS: connection got terminated");
  else if (ret < 0)
    throw (exceptions::msg() << "TLS: could not receive data: "
           << gnutls_strerror(ret));
  buffer->resize(ret);
  d = buffer;

  return (true);
}

/**
 *  Read encrypted data from base stream.
 *
 *  @param[out] buffer Output buffer.
 *  @param[in]  size   Maximum size.
 *
 *  @return Number of bytes actually read.
 */
unsigned int stream::read_encrypted(void* buffer, unsigned int size) {
  // Read some data.
  while (_buffer.isEmpty()) {
    misc::shared_ptr<io::data> d;
    _substream->read(d);
    if (!d.isNull() && (d->type() == io::raw::static_type())) {
      io::raw* r(static_cast<io::raw*>(d.data()));
      _buffer.append(r->QByteArray::data(), r->size());
    }
  }

  // Transfer data.
  unsigned int rb(_buffer.size());
  if (size >= rb) {
    memcpy(buffer, _buffer.data(), rb);
    _buffer.clear();
  }
  else {
    memcpy(buffer, _buffer.data(), size);
    _buffer.remove(0, size);
    rb = size;
  }
  return (rb);
}

/**
 *  @brief Send data across the TLS session.
 *
 *  Send a chunk of data.
 *
 *  @param[in] d Packet to send.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  // Send data.
  if (!d.isNull() && d->type() == io::raw::static_type()) {
    io::raw const* packet(static_cast<io::raw const*>(d.data()));
    char const* ptr(packet->QByteArray::data());
    int size(packet->size());
    while (size > 0) {
      int ret(gnutls_record_send(
                *_session,
                ptr,
                size));
      if (ret < 0)
        throw (exceptions::msg() << "TLS: could not send data: "
               << gnutls_strerror(ret));
      ptr += ret;
      size -= ret;
    }
  }

  return (1);
}

/**
 *  Write encrypted data to base stream.
 *
 *  @param[in] buffer Data to write.
 *  @param[in] size   Size of buffer.
 *
 *  @return Number of bytes written.
 */
unsigned int stream::write_encrypted(
                       void const* buffer,
                       unsigned int size) {
  misc::shared_ptr<io::raw> r(new io::raw);
  r->append(static_cast<char const*>(buffer), size);
  _substream->write(r);
  _substream->write(misc::shared_ptr<io::data>());
  return (size);
}
