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

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
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
        throw msg_fmt("Unable to allocate acceptor SSL object");

      if (!_cert.empty() && !_key.empty()) {
        int r;
        log_v2::tls()->info("TLS: using certificates as credentials");

        /* Force TLS hostname */
        if (!_tls_hostname.empty()) {
          r = SSL_set_tlsext_host_name(s_ssl, _tls_hostname.c_str());
          if (r != 1)
            throw msg_fmt("Error: cannot set tls hostname '{}'", _tls_hostname);
        }

        /* Load CA certificate */
        if (!_ca.empty()) {
          r = SSL_use_certificate_chain_file(s_ssl, _ca.c_str());
          if (r != 1)
            throw msg_fmt("Error: cannot load trusted certificate authority's file '{}'", _ca);
        }

        /* Load certificate */
        r = SSL_use_certificate_file(s_ssl, _cert.c_str(), SSL_FILETYPE_PEM);
        if (r <= 0)
          throw msg_fmt("Error: cannot load certificate file '{}'", _cert);

        /* Load private key */
        r = SSL_use_PrivateKey_file(s_ssl, _key.c_str(), SSL_FILETYPE_PEM);
        if (r <= 0)
          throw msg_fmt("Error: cannot load private key file '{}'", _key);

        /* Check if the private key is valid */
        r = SSL_check_private_key(s_ssl);
        if (r != 1)
          throw msg_fmt("Error: checking the private key '{}' failed.", _key);

        if (!SSL_set_cipher_list(s_ssl, "HIGH"))
          throw msg_fmt("Error: cannot set the cipher list to HIGH");
      }
      else {
        log_v2::tls()->info("TLS: using anonymous server credentials");
        SSL_set_security_level(s_ssl, 0);
        if (!SSL_set_cipher_list(s_ssl, "aNULL"))
          throw msg_fmt("Error: cannot set the cipher list to HIGH");
      }

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
