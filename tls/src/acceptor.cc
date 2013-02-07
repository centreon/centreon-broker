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

#include <gnutls/gnutls.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tls/acceptor.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/params.hh"
#include "com/centreon/broker/tls/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
acceptor::acceptor(
            std::string const& cert,
            std::string const& key,
            std::string const& ca)
  : io::endpoint(true), _ca(ca), _cert(cert), _key(key) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
acceptor::acceptor(acceptor const& right) : io::endpoint(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {}

/**
 *  Assignement operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& right) {
  if (this != &right) {
    io::endpoint::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return A clone of this object.
 */
io::endpoint* acceptor::clone() const {
  return (new acceptor(*this));
}

/**
 *  Close the acceptor.
 */
void acceptor::close() {
  if (!_from.isNull())
    _from->close();
  return ;
}

/**
 *  @brief Try to accept a new connection.
 *
 *  Wait for an incoming client through the underlying acceptor, perform
 *  TLS checks (if configured to do so) and return a TLS encrypted
 *  stream.
 *
 *  @return A TLS-encrypted stream (namely a tls::stream object).
 *
 *  @see tls::stream
 */
misc::shared_ptr<io::stream> acceptor::open() {
  /*
  ** The process of accepting a TLS client is pretty straight-forward.
  ** Just follow the comments the have an overview of performed
  ** operations.
  */

  // First accept a client from the lower layer.
  misc::shared_ptr<io::stream> lower(_from->open());
  misc::shared_ptr<io::stream> new_stream;
  if (!lower.isNull())
    new_stream = open(lower);
  return (new_stream);
}

/**
 *  Overload of open, using base stream.
 *
 *  @param[in] lower Open stream.
 *
 *  @return Encrypted stream.
 */
misc::shared_ptr<io::stream> acceptor::open(
                                         misc::shared_ptr<io::stream> lower) {
  misc::shared_ptr<io::stream> s;
  int ret;
  if (!lower.isNull()) {
    // Load parameters.
    params p(params::SERVER);
    p.set_cert(_cert, _key);
    p.set_trusted_ca(_ca);
    p.load();

    gnutls_session_t* session(new gnutls_session_t);
    try {
      // Initialize the TLS session
      logging::debug(logging::low) << "TLS: initializing session";
      ret = gnutls_init(session, GNUTLS_SERVER);
      if (ret != GNUTLS_E_SUCCESS)
	throw (exceptions::msg() << "TLS: cannot initialize session: "
               << gnutls_strerror(ret));

      // Apply TLS parameters.
      p.apply(*session);

      // Create stream object.
      s = misc::shared_ptr<io::stream>(new stream(session));
    }
    catch (...) {
      gnutls_deinit(*session);
      delete (session);
      throw ;
    }
    s->read_from(lower);
    s->write_to(lower);

    // Bind the TLS session with the stream from the lower layer.
#if GNUTLS_VERSION_NUMBER < 0x020C00
    gnutls_transport_set_lowat(*session, 0);
#endif // GNU TLS < 2.12.0
    gnutls_transport_set_pull_function(*session, pull_helper);
    gnutls_transport_set_push_function(*session, push_helper);
    gnutls_transport_set_ptr(*session, s.data());

    // Perform the TLS handshake.
    logging::debug(logging::medium) << "TLS: performing handshake";
    do {
      ret = gnutls_handshake(*session);
    } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
    if (ret != GNUTLS_E_SUCCESS)
      throw (exceptions::msg() << "TLS: handshake failed: "
             << gnutls_strerror(ret));
    logging::debug(logging::medium) << "TLS: successful handshake";

    // Check certificate.
    p.validate_cert(*session);
  }

  return (s);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] Right Object to copy.
 */
void acceptor::_internal_copy(acceptor const& right) {
  _ca = right._ca;
  _cert = right._cert;
  _key = right._key;
  return ;
}
