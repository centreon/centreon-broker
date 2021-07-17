/*
** Copyright 2009-2013, 2021 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/tls/acceptor.hh"
#include <assert.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string.h>
#include <string>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/params.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;
using namespace com::centreon::broker::tls;

/**
 *  Default constructor.
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
acceptor::acceptor(std::string cert,
                   std::string key,
                   std::string ca,
                   std::string tls_hostname)
    : io::endpoint(true),
      _ca(std::move(ca)),
      _cert(std::move(cert)),
      _key(std::move(key)),
      _tls_hostname(std::move(tls_hostname)) {}

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
std::unique_ptr<io::stream> acceptor::open() {
  /*
  ** The process of accepting a TLS client is pretty straight-forward.
  ** Just follow the comments the have an overview of performed
  ** operations.
  */

  // First accept a client from the lower layer.
  std::unique_ptr<io::stream> lower(_from->open());
  if (lower)
    return open(std::move(lower));
  return nullptr;
}

/**
 *  Overload of open, using base stream.
 *
 *  @param[in] lower Open stream.
 *
 *  @return Encrypted stream.
 */
std::unique_ptr<io::stream> acceptor::open(std::shared_ptr<io::stream> lower) {
  std::unique_ptr<stream> u;
  if (lower) {
    try {
      SSL* s_ssl = SSL_new(tls::ctx);
      if (s_ssl == nullptr)
        throw msg_fmt("Unable to allocate acceptor ssl object");

      BIO *s_bio = nullptr, *server = nullptr, *s_bio_io = nullptr;

      // size_t bufsiz = 2048; /* small buffer for testing */

      if (!BIO_new_bio_pair(&server, 0 /*bufsiz*/, &s_bio_io, 0 /*bufsiz*/))
        throw msg_fmt("Unable to build SSL pair.");

      s_bio = BIO_new(BIO_f_ssl());
      if (!s_bio)
        throw msg_fmt("Unable to build SSL filter.");

      SSL_set_accept_state(s_ssl);
      SSL_set_bio(s_ssl, server, server);
      BIO_set_ssl(s_bio, s_ssl, BIO_NOCLOSE);

      // Create stream object.
      u = std::make_unique<stream>(s_ssl, s_bio, s_bio_io);
    } catch (...) {
      // FIXME DBR: memory leak
      throw;
    }
    u->set_substream(lower);

    /* Handshake as server */
    log_v2::tls()->error("tls before server handshake");
    u->handshake();
    log_v2::tls()->error("tls after server handshake");
  }

  return u;
}
