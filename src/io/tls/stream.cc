/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <stdlib.h>
#include "exceptions/retval.hh"
#include "io/stream.hh"
#include "io/tls/stream.hh"

using namespace io::tls;

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
  assert(false);
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
 *  @brief Constructor.
 *
 *  When building the stream, you need to provide a stream that will be
 *  used to transport encrypted data and a TLS session, providing
 *  informations on the kind of encryption to use. Upon completion of
 *  this constructor, the stream object is considered to be the owner of
 *  the given objects, which means that the stream object is responsible
 *  for their destruction.
 *
 *  @param[in] lower The stream object that will transport encrypted data.
 *  @param[in] sess  TLS session, providing informations on the
 *                   encryption that should be used.
 */
stream::stream(io::stream* lower, gnutls_session_t* sess)
  : _lower(lower), session(sess){}

/**
 *  @brief Destructor.
 *
 *  The destructor will release all acquired ressources that haven't
 *  been released yet.
 */
stream::~stream() {
  close();
}

/**
 *  @brief Close the TLS stream.
 *
 *  This method will shutdown the TLS session and close the underlying
 *  stream, releasing all acquired ressources.
 */
void stream::close() {
  if (session) {
    gnutls_bye(*session, GNUTLS_SHUT_RDWR);
    gnutls_deinit(*session);
    delete (session);
    session = NULL;
  }
  if (_lower.get()) {
    _lower->close();
    _lower.reset();
  }
  return ;
}

/**
 *  @brief Receive data from the TLS session.
 *
 *  Receive at most size bytes from the network stream and store them in
 *  buffer. The number of bytes read is then returned. This number can
 *  be less than size.
 *
 *  @param[out] buffer Buffer on which to store received data.
 *  @param[in]  size   Maximum number of bytes to read.
 *
 *  @return Number of bytes read from the network stream. 0 if the
 *          session has been shut down.
 */
unsigned int stream::receive(void* buffer, unsigned int size) {
  int ret(gnutls_record_recv(*session, buffer, size));
  if (ret < 0)
    throw (exceptions::retval(ret) << "could not receive TLS data: "
                                   << gnutls_strerror(ret));
  return (static_cast<unsigned int>(ret));
}

/**
 *  @brief Send data across the TLS session.
 *
 *  Send at most size bytes from the buffer. The number of bytes
 *  actually sent is returned. This number can be less than size.
 *
 *  @param[in] buffer Data to send.
 *  @param[in] size   Maximum number of bytes to send.
 *
 *  @return Number of bytes actually sent through the TLS session. 0 if
 *          the connection has been shut down.
 */
unsigned int stream::send(void const* buffer, unsigned int size) {
  int ret(gnutls_record_send(*session, buffer, size));
  if (ret < 0)
    throw (exceptions::retval(ret) << "could not send TLS data: "
                                   << gnutls_strerror(ret));
  return (static_cast<unsigned int>(ret));
}
