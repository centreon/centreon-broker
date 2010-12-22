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
#include <gnutls/gnutls.h>
#include "exceptions/basic.hh"
#include "exceptions/retval.hh"
#include "io/stream.hh"
#include "io/tls/acceptor.hh"
#include "io/tls/internal.hh"
#include "io/tls/stream.hh"

using namespace io::tls;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  As io::tls::acceptor is not copiable, the copy constructor is
 *  declared private. Any attempt to use it will result in a call to
 *  abort().
 *
 *  @param[in] a Unused.
 */
acceptor::acceptor(acceptor const& a)
  : io::acceptor(a), params(params::SERVER) {
  (void)a;
  assert(false);
  abort();
}

/**
 *  @brief Assignement operator.
 *
 *  As io::tls::acceptor is not copiable, the assignment operator is
 *  declared private. Any attempt to use it will result in a call to
 *  abort().
 *
 *  @param[in] a Unused.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  (void)a;
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
 *  Default constructor.
 */
acceptor::acceptor() : params(params::SERVER) {}

/**
 *  @brief Destructor.
 *
 *  Release all acquired ressources like Diffie-Hellman parameters,
 *  credentials, ... and close the underlying acceptor if it has not
 *  already been closed.
 */
acceptor::~acceptor() {
  close();
}

/**
 *  @brief Try to accept a new connection.
 *
 *  Wait for an incoming client through the underlying acceptor, perform
 *  TLS checks (if configured to do so) and return a TLS encrypted
 *  stream.
 *
 *  @return A TLS-encrypted stream (namely an io::tls::stream object).
 *
 *  @see io::tls::stream
 */
io::stream* acceptor::accept() {
  /*
  ** The process of accepting a TLS client is pretty straight-forward.
  ** Just follow the comments the have an overview of performed
  ** operations.
  */

  // First accept a client from the lower layer.
  io::stream* lower(_lower->accept());
  gnutls_session_t* session(NULL);
  stream* s(NULL);
  if (lower) {
    try {
      int ret;

      // Initialize the TLS session
      session = new (gnutls_session_t);
      ret = gnutls_init(session, GNUTLS_SERVER);
      if (ret != GNUTLS_E_SUCCESS)
	throw (exceptions::retval(ret) << "TLS error: "
                                       << gnutls_strerror(ret));

      // Apply TLS parameters.
      apply(*session);

      // Bind the TLS session with the stream from the lower layer.
      gnutls_transport_set_lowat(*session, 0);
      gnutls_transport_set_pull_function(*session, pull_helper);
      gnutls_transport_set_push_function(*session, push_helper);
      gnutls_transport_set_ptr(*session, lower);

      // Perform the TLS handshake.
      do {
	ret = gnutls_handshake(*session);
      } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
      if (ret != GNUTLS_E_SUCCESS)
	throw (exceptions::retval(ret) << "TLS error: "
                                       << gnutls_strerror(ret));

      // Check certificate.
      if (check_cert(*session))
	throw (exceptions::basic() << "invalid certificate used in "	\
                                      "TLS connection");

      s = new stream(lower, session);
    }
    catch (...) {
      if (session) {
	gnutls_deinit(*session);
	delete (session);
      }
      lower->close();
      delete (lower);
      throw ;
    }
  }

  return (s);
}

/**
 *  @brief Close the acceptor.
 *
 *  Release all acquired ressources and close the underlying acceptor.
 */
void acceptor::close() {
  if (_lower.get()) {
    _lower->close();
    _lower.reset();
  }
  return ;
}

/**
 *  @brief Get the acceptor ready.
 *
 *  Initialize late TLS-related objects. From now on, users can call
 *  accept() to wait for new clients. Upon a successful return of this
 *  method, the acceptor object is considered owning the io::acceptor
 *  object provided, which means that the tls_acceptor object will
 *  handle its destruction.
 *
 *  @param[in] lower The underlying acceptor.
 */
void acceptor::listen(io::acceptor* lower) {
  _lower.reset(lower);
  return ;
}
