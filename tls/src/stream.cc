/*
** Copyright 2009-2013 Merethis
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

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "com/centreon/broker/exceptions/msg.hh"
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
stream::stream(gnutls_session_t* sess)
  : _process_in(true), _process_out(true), _session(sess) {}

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
 *  Set which data to process.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void stream::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
  return ;
}

/**
 *  @brief Receive data from the TLS session.
 *
 *  Receive at most size bytes from the network stream and store them in
 *  buffer. The number of bytes read is then returned. This number can
 *  be less than size.
 *
 *  @param[out] d  Object that will be returned containing a chunk of
 *                 data.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  // Clear existing content.
  d.clear();

  // Check that data should be processed.
  if (!_process_in)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "TLS stream is shutdown");

  // Read data.
  misc::shared_ptr<io::raw> buffer(new io::raw);
  buffer->resize(BUFSIZ);
  int ret(gnutls_record_recv(
            *_session,
            buffer->QByteArray::data(),
            buffer->size()));
  if (ret < 0)
    throw (exceptions::msg() << "TLS: could not receive data: "
           << gnutls_strerror(ret));
  if (ret > 0) {
    buffer->resize(ret);
    d = buffer.staticCast<io::data>();
  }

  return ;
}

/**
 *  @brief Send data across the TLS session.
 *
 *  Send a chunk of data.
 *
 *  @param[in] d Packet to send.
 */
void stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "TLS stream is shutdown");

  // Send data.
  static QString const raw_type("com::centreon::broker::io::raw");
  if (!d.isNull() && d->type() == raw_type) {
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

  return ;
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  stream is not copiable, that's why the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  @param[in] s Unused.
 */
stream::stream(stream const& s) : io::stream(s) {
  assert(!"TLS stream is not copyable");
  abort();
}

/**
 *  @brief Assignement operator.
 *
 *  stream is not copiable, that's why the assignment operator is
 *  declared private. Any attempt to use it will result in a call to
 *  abort().
 *
 *  @param[in] s Unused.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  (void)s;
  assert(!"TLS stream is not copyable");
  abort();
  return (*this);
}
