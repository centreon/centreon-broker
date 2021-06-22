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
#include <gnutls/gnutls.h>
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
acceptor::acceptor(const std::string& cert,
                   const std::string& key,
                   const std::string& ca,
                   const std::string& tls_hostname)
    : io::endpoint(true),
      _ca(ca),
      _cert(cert),
      _key(key),
      _tls_hostname(tls_hostname) {}

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
  std::unique_ptr<io::stream> u;
  if (lower) {
    try {
      SSL_CTX* s_ctx = SSL_CTX_new(TLS_server_method());

      SSL_CTX_set_security_level(s_ctx, 0);

      SSL_CTX_set_ecdh_auto(s_ctx, 1);

      if (!SSL_CTX_set_cipher_list(s_ctx, "aNULL")) {
        log_v2::tls()->error("new acceptor cipher list error");
      }

      SSL* s_ssl = SSL_new(s_ctx);

      BIO* s_ssl_bio = nullptr;

      BIO *server = nullptr, *server_io = nullptr;

      int err_in_server = 0;

      size_t bufsiz = 2048; /* small buffer for testing */

      if (!BIO_new_bio_pair(&server, bufsiz, &server_io, bufsiz))
        log_v2::tls()->error("new crash ");

      s_ssl_bio = BIO_new(BIO_f_ssl());
      if (!s_ssl_bio)
        log_v2::tls()->error("new acceptor ssl bio error");

      SSL_set_accept_state(s_ssl);
      SSL_set_bio(s_ssl, server, server);
      (void)BIO_set_ssl(s_ssl_bio, s_ssl, BIO_NOCLOSE);
      // Create stream object.
      log_v2::tls()->error("tls juste avant le stream acceptor");
      u.reset(new stream(s_ssl, s_ssl_bio, server_io));
    } catch (...) {
      // gnutls_deinit(*session);
      // delete session;
      throw;
    }
    u->set_substream(lower);
  }

  return u;
}
