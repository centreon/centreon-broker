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

#include "com/centreon/broker/tls2/connector.hh"

#include <openssl/x509v3.h>
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls2/internal.hh"
#include "com/centreon/broker/tls2/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls2;
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

static int verify_callback(int preverify_ok, X509_STORE_CTX* ctx) {
  char buf[256];
  X509* err_cert;
  int err, depth;

  err_cert = X509_STORE_CTX_get_current_cert(ctx);
  err = X509_STORE_CTX_get_error(ctx);
  depth = X509_STORE_CTX_get_error_depth(ctx);

  /*
   * Retrieve the pointer to the SSL of the connection currently treated
   * and the application specific data stored into the SSL object.
   */
  //ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
  //mydata = SSL_get_ex_data(ssl, mydata_index);

  X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);

  /*
   * Catch a too long certificate chain. The depth limit set using
   * SSL_CTX_set_verify_depth() is by purpose set to "limit+1" so
   * that whenever the "depth>verify_depth" condition is met, we
   * have violated the limit and want to log this error condition.
   * We must do it here, because the CHAIN_TOO_LONG error would not
   * be found explicitly; only errors introduced by cutting off the
   * additional certificates would be logged.
   */
  log_v2::tls()->info("depth = {}", depth);
//  if (depth > mydata->verify_depth) {
//    preverify_ok = 0;
//    err = X509_V_ERR_CERT_CHAIN_TOO_LONG;
//    X509_STORE_CTX_set_error(ctx, err);
//  }
  if (!preverify_ok) {
    log_v2::tls()->error("verify error:num={}:{}:depth={}:{}", err,
           X509_verify_cert_error_string(err), depth, buf);
  }

  /*
   * At this point, err contains the last verification error. We can use
   * it for something special
   */
  if (!preverify_ok && (err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT)) {
    X509_NAME_oneline(X509_get_issuer_name(err_cert), buf, 256);
    printf("issuer= %s\n", buf);
  }

//  if (mydata->always_continue)
//    return 1;
//  else
    return preverify_ok;
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
      /* First, we clear SSL errors */
      ERR_clear_error();

      SSL* c_ssl = SSL_new(tls2::ctx);
      if (c_ssl == nullptr)
        throw msg_fmt("Unable to allocate connector ssl object");

      if (!_cert.empty() && !_key.empty()) {
        int r;
        log_v2::tls()->info("TLS: using certificates as credentials");

        X509_VERIFY_PARAM* ssl_params = SSL_get0_param(c_ssl);
        X509_VERIFY_PARAM_set_hostflags(ssl_params,
                                        X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);

        SSL_set_mode(c_ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER | SSL_MODE_AUTO_RETRY);

        /* Force TLS hostname */
        int mode;
        if (!_tls_hostname.empty()) {
          if (!SSL_set_tlsext_host_name(c_ssl, _tls_hostname.c_str()))
            throw msg_fmt("Error: cannot set tls2 hostname '{}'", _tls_hostname);

          if (!X509_VERIFY_PARAM_set1_host(ssl_params, _tls_hostname.c_str(), _tls_hostname.size()))
            throw msg_fmt("Error: cannot set tls2 host name '{}' to X509 parameters", _tls_hostname);
          mode = SSL_VERIFY_PEER;
        }
        else
          mode = SSL_VERIFY_NONE;
        SSL_set_verify(c_ssl, mode, verify_callback);

        if (
#ifdef TLS1_3_VERSION
            !SSL_set_ciphersuites(c_ssl, "HIGH") &&
#endif
            !SSL_set_cipher_list(c_ssl, "HIGH"))
          throw msg_fmt("Error: cannot set the cipher list to HIGH");

        /* Load CA certificate */
        if (!_ca.empty()) {
          r = SSL_use_certificate_chain_file(c_ssl, _ca.c_str());
          if (r != 1)
            throw msg_fmt(
                "Error: cannot load trusted certificate authority's file '{}'",
                _ca);
        }

        /* Load certificate */
        if (SSL_use_certificate_file(c_ssl, _cert.c_str(), SSL_FILETYPE_PEM) != 1)
          throw msg_fmt("Error: cannot load certificate file '{}'", _cert);

        /* Load private key */
        if (SSL_use_PrivateKey_file(c_ssl, _key.c_str(), SSL_FILETYPE_PEM) != 1)
          throw msg_fmt("Error: cannot load private key file '{}'", _key);

        /* Check if the private key is valid */
        if (SSL_check_private_key(c_ssl) != 1)
          throw msg_fmt("Error: checking the private key '{}' failed.", _key);

      } else {
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
      u = std::make_unique<stream>(c_ssl, c_bio, c_bio_io, false);
    } catch (...) {
      // delete c_ssl;
      throw;
    }
    u->set_substream(lower);

    /* Handshake as connector */
    u->handshake();
  }

  return u;
}
