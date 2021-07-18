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

#include "com/centreon/broker/tls/connector.hh"

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;
using namespace com::centreon::exceptions;

/**
 *  Default constructor
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
connector::connector(std::string cert,
                     std::string key,
                     std::string ca,
                     std::string tls_hostname)
    : io::endpoint(false),
      _ca(std::move(ca)),
      _cert(std::move(cert)),
      _key(std::move(key)),
      _tls_hostname(std::move(tls_hostname)) {}

/**
 *  Connect to the remote TLS peer.
 *
 *  @return New connected stream.
 */
std::unique_ptr<io::stream> connector::open() {
  // First connect the lower layer.
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
std::unique_ptr<io::stream> connector::open(std::shared_ptr<io::stream> lower) {
  std::unique_ptr<stream> u;
  if (lower) {
    try {
      SSL* c_ssl = SSL_new(tls::ctx);
      if (c_ssl == nullptr)
        throw msg_fmt("Unable to allocate connector ssl object");

      if (!_cert.empty() && !_key.empty()) {
        int r;
        log_v2::tls()->info("TLS: using certificates as credentials");

        /* Force TLS hostname */
        if (!_tls_hostname.empty()) {
          r = SSL_set_tlsext_host_name(c_ssl, _tls_hostname.c_str());
          if (r != 1)
            throw msg_fmt("Error: cannot set tls hostname '{}'", _tls_hostname);
        }

        /* Load CA certificate */
        if (!_ca.empty()) {
          r = SSL_use_certificate_chain_file(c_ssl, _ca.c_str());
          if (r <= 0)
            throw msg_fmt("Error: cannot load trusted certificate authority's file '{}'", _ca);
        }

        /* Load certificate */
        r = SSL_use_certificate_file(c_ssl, _cert.c_str(), SSL_FILETYPE_PEM);
        if (r <= 0)
          throw msg_fmt("Error: cannot load certificate file '{}'", _cert);

        /* Load private key */
        r = SSL_use_PrivateKey_file(c_ssl, _key.c_str(), SSL_FILETYPE_PEM);
        if (r <= 0)
          throw msg_fmt("Error: cannot load private key file '{}'", _key);

        /* Check if the private key is valid */
        r = SSL_check_private_key(c_ssl);
        if (r != 1)
          throw msg_fmt("Error: checking the private key '{}' failed.", _key);

        if (!SSL_set_cipher_list(c_ssl, "HIGH"))
          throw msg_fmt("Error: cannot set the cipher list to HIGH");
      }
      else {
        log_v2::tls()->info("TLS: using anonymous client credentials");
        SSL_set_security_level(c_ssl, 0);
        if (!SSL_set_cipher_list(c_ssl, "aNULL"))
          throw msg_fmt("Error: cannot set the cipher list to HIGH");
      }

      BIO *c_bio = nullptr, *client = nullptr, *c_bio_io = nullptr;

      // size_t bufsiz = 2048; /* small buffer for testing */

      if (!BIO_new_bio_pair(&client, 0 /*bufsiz*/, &c_bio_io, 0 /*bufsiz*/))
        throw msg_fmt("Unable to build SSL pair.");

      c_bio = BIO_new(BIO_f_ssl());
      if (!c_bio)
        throw msg_fmt("Unable to build SSL filter.");

      SSL_set_connect_state(c_ssl);
      SSL_set_bio(c_ssl, client, client);
      (void)BIO_set_ssl(c_bio, c_ssl, BIO_NOCLOSE);

      // Create stream object.
      u = std::make_unique<stream>(c_ssl, c_bio, c_bio_io);
    } catch (...) {
      // delete c_ssl;
      throw;
    }
    u->set_substream(lower);

    /* Handshake as connector */
    log_v2::tls()->error("tls before handshake");
    u->handshake();
    log_v2::tls()->error("tls after handshake");
  }

  return u;
}
