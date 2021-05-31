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

#include <gnutls/gnutls.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;
using namespace com::centreon::broker::tls;

static std::string err_as_string(void) {
  BIO* bio = BIO_new(BIO_s_mem());
  ERR_print_errors(bio);
  char* buf = nullptr;
  size_t len = BIO_get_mem_data(bio, &buf);
  std::string retval{buf, len};
  BIO_free(bio);
  return retval;
}

/**
 *  Default constructor.
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
acceptor::acceptor(std::string const& cert,
                   std::string const& key,
                   std::string const& ca,
                   std::string const& tls_hostname)
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
  std::unique_ptr<tls::stream> u;
  if (lower) {
    int ret;

    // Load parameters.
    //    params p(params::SERVER);
    //    p.set_cert(_cert, _key);
    //    p.set_trusted_ca(_ca);
    //    p.set_tls_hostname(_tls_hostname);
    //    p.load();

    SSL* ssl = SSL_new(tls::ctx);

    SSL_CTX_set_ecdh_auto(tls::ctx, 1);
    //SSL_set_accept_state(ssl);

    std::string err;

    // gnutls_session_t* session(new gnutls_session_t);
    try {
      // Initialize the TLS session
      log_v2::tls()->debug("TLS: initializing session");
      // GNUTLS_NONBLOCK was introduced in gnutls 2.99.3.
      //#ifdef GNUTLS_NONBLOCK
      //      ret = gnutls_init(session, GNUTLS_SERVER | GNUTLS_NONBLOCK);
      //#else
      //      ret = gnutls_init(session, GNUTLS_SERVER);
      //#endif  // GNUTLS_NONBLOCK
      //      if (ret != GNUTLS_E_SUCCESS) {
      //        log_v2::tls()->error("TLS: cannot initialize session: {}",
      //                             gnutls_strerror(ret));
      //        throw msg_fmt("TLS: cannot initialize session: {}",
      //                      gnutls_strerror(ret));
      //      }
      //
      //      // Apply TLS parameters.
      //      p.apply(*session);

      if (_cert.empty() || _key.empty()) {
        log_v2::tls()->info("TLS: using anonymous server credentials");
        ret = SSL_set_cipher_list(ssl, "aNULL");
        if (!ret)
          throw msg_fmt("TLS: unable to find cypher for anonymous session");
      } else {
        if (SSL_use_certificate_file(ssl, _cert.c_str(), SSL_FILETYPE_PEM) <=
            0) {
          throw msg_fmt("TLS: unable to load certificate '{}': {}", _cert,
                        err_as_string());
        }
        if (SSL_use_PrivateKey_file(ssl, _key.c_str(), SSL_FILETYPE_PEM) <= 0) {
          throw msg_fmt("TLS: unable to load private key '{}': {}", _key,
                        err_as_string());
        }

        ret = SSL_set_cipher_list(ssl, "HIGH:!aNULL");
        if (!ret)
          throw msg_fmt("TLS: Unable to set cipher list: {}", err_as_string());
      }

      // Create stream object.
      u = std::make_unique<stream>(ssl, true);
    } catch (...) {
      SSL_free(ssl);
      throw;
    }
    u->set_substream(lower);

    //    // Bind the TLS session with the stream from the lower layer.
    //#if GNUTLS_VERSION_NUMBER < 0x020C00
    //    gnutls_transport_set_lowat(*session, 0);
    //#endif  // GNU TLS < 2.12.0
    //    gnutls_transport_set_pull_function(*session, pull_helper);
    //    gnutls_transport_set_push_function(*session, push_helper);
    //    gnutls_transport_set_ptr(*session, u.get());

    // Perform the TLS handshake.
    do {
      ret = u->handshake();
    } while (ret == -1);
    if (ret == -2)
      throw msg_fmt("TLS: handshake failed");

    log_v2::tls()->debug("TLS: successful handshake");
    SSL_SESSION* session = SSL_get_session(ssl);
    const char* version = SSL_get_version(ssl);
    const SSL_CIPHER* cipher = SSL_SESSION_get0_cipher(session);

    log_v2::tls()->info(
        "TLS: protocol '{}' and cipher '{}' used", version,
        cipher ? SSL_CIPHER_get_name(cipher) : "unknown cipher");

    // Check certificate.
    //    p.validate_cert(*session);
  }

  return u;
}
