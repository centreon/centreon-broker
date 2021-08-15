/*
** Copyright 2021 Centreon
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

#include "com/centreon/broker/tls2/acceptor.hh"
#include <assert.h>
#include <openssl/x509v3.h>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls2/internal.hh"
#include "com/centreon/broker/tls2/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;
using namespace com::centreon::broker::tls2;

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
 *  @return A TLS-encrypted stream (namely a tls2::stream object).
 *
 *  @see tls2::stream
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

static void info_callback(const SSL* s, int where, int ret) {
  const char* str1;

  int w = where & ~SSL_ST_MASK;

  if (w & SSL_ST_CONNECT)
    str1 = "INFO CLIENT";
  else if (w & SSL_ST_ACCEPT)
    str1 = "INFO SERVER";
  else
    str1 = "undefined";

  if (where & SSL_CB_LOOP) {
    log_v2::tls()->info("{}:{}", str1, SSL_state_string_long(s));
  } else if (where & SSL_CB_ALERT) {
    const char* str = (where & SSL_CB_READ) ? "read" : "write";
    log_v2::tls()->info("{}:SSL3 alert {}:{}:{}", str1, str,
                        SSL_alert_type_string_long(ret),
                        SSL_alert_desc_string_long(ret));
  } else if (where & SSL_CB_EXIT) {
    if (ret == 0)
      log_v2::tls()->info("{}: failed in {}", str1, SSL_state_string_long(s));
    else if (ret < 0) {
      log_v2::tls()->info("{}:error in {}", str1, SSL_state_string_long(s));
    }
  }
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
      /* First we clear SSL errors */
      ERR_clear_error();

      SSL* s_ssl = SSL_new(tls2::ctx);
      if (s_ssl == nullptr)
        throw msg_fmt("Unable to allocate acceptor SSL object");

      SSL_set_info_callback(s_ssl, info_callback);

      if (!_tls_hostname.empty())
        throw msg_fmt("Error: cannot set tls hostname on the acceptor side.");

      /* Load CA certificate */
      if (!_ca.empty()) {
        if (SSL_CTX_load_verify_locations(tls2::ctx, _ca.c_str(), nullptr) != 1)
          throw msg_fmt(
              "Error: cannot load trusted certificate authority's file '{}': {}",
              _ca, ERR_reason_error_string(ERR_get_error()));
      }

      if (!_cert.empty() && !_key.empty()) {
        log_v2::tls()->info("TLS: using certificates as credentials");

        /* Load private key */
        if (SSL_use_PrivateKey_file(s_ssl, _key.c_str(), SSL_FILETYPE_PEM) != 1)
          throw msg_fmt("Error: cannot load private key file '{}'", _key);

        /* Load certificate */
        if (SSL_CTX_use_certificate_chain_file(tls2::ctx, _cert.c_str()) != 1 ||
            SSL_use_certificate_file(s_ssl, _cert.c_str(), SSL_FILETYPE_PEM) != 1)
          throw msg_fmt("Error: cannot load certificate file '{}'", _cert);

        /* Check if the private key is valid */
        if (SSL_check_private_key(s_ssl) != 1)
          throw msg_fmt("Error: checking the private key '{}' failed.", _key);

        if (!SSL_set_cipher_list(s_ssl, "HIGH"))
          throw msg_fmt("Error: cannot set the cipher list to HIGH");

        /* Force TLS hostname */
      } else {
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
      u = std::make_unique<stream>(s_ssl, s_bio, s_bio_io, true);
    } catch (...) {
      // FIXME DBR: memory leak
      throw;
    }
    u->set_substream(lower);

    /* Handshake as server */
    u->handshake();
  }

  return u;
}
