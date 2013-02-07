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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/tls/connector.hh"
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
 *  Default constructor
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
connector::connector(
             std::string const& cert,
             std::string const& key,
             std::string const& ca)
  : io::endpoint(false), _ca(ca), _cert(cert), _key(key) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
connector::connector(connector const& right) : io::endpoint(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& right) {
  if (this != & right) {
    io::endpoint::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Connect to the remote TLS peer.
 *
 *  @return New connected stream.
 */
misc::shared_ptr<io::stream> connector::open() {
  // First connect the lower layer.
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
misc::shared_ptr<io::stream> connector::open(
                                          misc::shared_ptr<io::stream> lower) {
  misc::shared_ptr<io::stream> s;
  if (!lower.isNull()) {
    // Load parameters.
    params p(params::CLIENT);
    p.set_cert(_cert, _key);
    p.set_trusted_ca(_ca);
    p.load();

    gnutls_session_t* session(new gnutls_session_t);
    try {
      // Initialize the TLS session
      int ret;
      if ((ret = gnutls_init(session, GNUTLS_CLIENT))
          != GNUTLS_E_SUCCESS)
        throw (exceptions::msg() << "TLS: cannot initialize session: "
               << gnutls_strerror(ret));

      // Apply TLS parameters to the current session.
      p.apply(*session);

      // Bind the TLS session with the stream from the lower layer.
#if GNUTLS_VERSION_NUMBER < 0x020C00
      gnutls_transport_set_lowat(*session, 0);
#endif // GNU TLS < 2.12.0
      gnutls_transport_set_pull_function(*session, pull_helper);
      gnutls_transport_set_push_function(*session, push_helper);
      gnutls_transport_set_ptr(*session, lower.data());

      // Perform the TLS handshake.
      do {
	ret = gnutls_handshake(*session);
      } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
      if (ret != GNUTLS_E_SUCCESS)
	throw (exceptions::msg() << "TLS: handshake failed: "
               << gnutls_strerror(ret));

      // Check certificate if necessary.
      p.validate_cert(*session);

      // Create stream object.
      s = misc::shared_ptr<io::stream>(new stream(session));
      s->read_from(lower);
      s->write_to(lower);
    }
    catch (...) {
      gnutls_deinit(*session);
      delete (session);
      throw ;
    }
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
 *  @param[in] right Object to copy.
 */
void connector::_internal_copy(connector const& right) {
  _ca = right._ca;
  _cert = right._cert;
  _key = right._key;
  return ;
}
