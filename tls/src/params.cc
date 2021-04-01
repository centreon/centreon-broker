/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/tls/params.hh"

#include <gnutls/gnutls.h>

#include <cstdlib>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;
using namespace com::centreon::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Params constructor.
 *
 *  @param[in] type Either CLIENT or SERVER, depending on connection
 *                  initialization. This cannot be modified after
 *                  construction.
 */
params::params(params::connection_type type)
    : _compress(false), _init(false), _type(type) {}

/**
 *  Destructor.
 */
params::~params() {
  _clean();
}

/**
 *  Apply parameters to a GNU TLS session object.
 *
 *  @param[out] session Object on which parameters will be applied.
 */
void params::apply(gnutls_session_t session) {
  // // Set the encryption method (normal ciphers with anonymous
  // // Diffie-Hellman and optionnally compression).
  // int ret;
  // ret = gnutls_priority_set_direct(
  //     session,
  //     (_compress ? "NORMAL:+ANON-DH:%COMPAT"
  //                : "NORMAL:+ANON-DH:+COMP-DEFLATE:%COMPAT"),
  //     nullptr);
  // if (ret != GNUTLS_E_SUCCESS) {
  //   log_v2::tls()->error("TLS: encryption parameter application failed: {}",
  //                        gnutls_strerror(ret));
  //   throw msg_fmt("TLS: encryption parameter application failed: {}",
  //                 gnutls_strerror(ret));
  // }

  // // Set anonymous credentials...
  // if (_cert.empty() || _key.empty()) {
  //   if (CLIENT == _type) {
  //     log_v2::tls()->debug("TLS: using anonymous client credentials");
  //     ret = gnutls_credentials_set(session, GNUTLS_CRD_ANON, _cred.client);
  //   } else {
  //     log_v2::tls()->debug("TLS: using anonymous server credentials");
  //     ret = gnutls_credentials_set(session, GNUTLS_CRD_ANON, _cred.server);
  //   }
  // }
  // // ... or certificate credentials.
  // else {
  //   log_v2::tls()->debug("TLS: using certificates as credentials");
  //   ret = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE,
  //   _cred.cert); if (SERVER == _type)
  //     gnutls_certificate_server_set_request(session, GNUTLS_CERT_REQUEST);
  // }
  // if (ret != GNUTLS_E_SUCCESS) {
  //   log_v2::tls()->error("TLS: could not set credentials: {}",
  //                        gnutls_strerror(ret));
  //   throw msg_fmt("TLS: could not set credentials: {}",
  //   gnutls_strerror(ret));
  // }
}

/**
 *  Load TLS parameters.
 */
void params::load(SSL_CTX* ctx) {
  log_v2::tls()->info("TLS: on params load");
  // Certificate-based.
  if (!_cert.empty() && !_key.empty()) {
    log_v2::tls()->info("TLS: on params load1");
    // Initialize credentials.
    if (SSL_CTX_use_certificate_file(ctx, _cert.c_str(), SSL_FILETYPE_PEM) <=
        0) {
      // ERR_print_errors_fp(stderr);
      log_v2::tls()->info("TLS: on params load ratéee 1");
      abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if (SSL_CTX_use_PrivateKey_file(ctx, _key.c_str(), SSL_FILETYPE_PEM) <= 0) {
      // ERR_print_errors_fp(stderr);
      log_v2::tls()->info("TLS: on params load ratéee 2");
      abort();
    }
    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx)) {
      log_v2::tls()->info("TLS: on params loadratéee 3");
      fprintf(stderr, "Private key does not match the public certificate\n");
      abort();
    }

    if (!_ca.empty()) {
      // Load certificate.
      int ret = SSL_CTX_use_certificate_chain_file(ctx, _ca.c_str());
      if (ret <= 0) {
        log_v2::tls()->info("TLS: on params load ratéee 4");
      }
    }
  }
  // Anonymous.
}

/**
 *  @brief Reset parameters to their default values.
 *
 *  Parameters are changed back to the default anonymous mode without
 *  compression.
 */
void params::reset() {
  _clean();
}

/**
 *  @brief Set certificates to use for connection encryption.
 *
 *  Two encryption mode are provided : anonymous and certificate-based.
 *  If you want to use certificates for encryption, call this function
 *  with the name of the PEM-encoded public certificate (cert) and the
 *  private key (key).
 *
 *  @param[in] cert The path to the PEM-encoded public certificate.
 *  @param[in] key  The path to the PEM-encoded private key.
 */
void params::set_cert(std::string const& cert, std::string const& key) {
  _cert = cert;
  _key = key;
}

/**
 *  @brief Set the compression mode (on/off).
 *
 *  Determines whether or not the encrypted stream should also be
 *  compressed using the Deflate algorithm. This kind of compression
 *  usually works well on text or other compressible data. The
 *  compression algorithm, may be useful in high bandwidth TLS tunnels,
 *  and in cases where network usage has to be minimized. As a drawback,
 *  compression increases latency.
 *
 *  @param[in] compress true if the stream should be compressed, false
 *                      otherwise.
 */
void params::set_compression(bool compress) {
  _compress = compress;
}

/**
 *  @brief Set the trusted CA certificate.
 *
 *  If this parameter is set, certificate checking will be performed on
 *  the connection against this CA certificate.
 *
 *  @param[in] ca_cert The path to the PEM-encoded public certificate of
 *                     the trusted Certificate Authority.
 */
void params::set_trusted_ca(std::string const& ca_cert) {
  _ca = ca_cert;
}

/**
 *  @brief Check if the peer's certificate is valid.
 *
 *  Check if the certificate invalid or revoked or untrusted or
 *  insecure. In those case, the connection should not be trusted. If no
 *  certificate is used for this connection or no trusted CA has been
 *  set, the method will return false.
 *
 *  @param[in] session Session on which checks will be performed.
 */
void params::validate_cert(gnutls_session_t session) {
  // if (!_ca.empty()) {
  //   int ret;
  //   uint32_t status;
  //   ret = gnutls_certificate_verify_peers2(session, &status);
  //   if (ret != GNUTLS_E_SUCCESS) {
  //     log_v2::tls()->error(
  //         "TLS: certificate verification failed , assuming invalid "
  //         "certificate: {}",
  //         gnutls_strerror(ret));
  //     throw msg_fmt(
  //         "TLS: certificate verification failed, assuming invalid
  //         certificate: "
  //         "{}",
  //         gnutls_strerror(ret));
  //   } else if (status & GNUTLS_CERT_INVALID) {
  //     log_v2::tls()->error("TLS: peer certificate is invalid");
  //     throw msg_fmt("TLS: peer certificate is invalid");
  //   } else if (status & GNUTLS_CERT_REVOKED) {
  //     log_v2::tls()->error("TLS: peer certificate was revoked");
  //     throw msg_fmt("TLS: peer certificate was revoked");
  //   } else if (status & GNUTLS_CERT_SIGNER_NOT_FOUND) {
  //     log_v2::tls()->error(
  //         "TLS: peer certificate was not issued by a trusted authority");
  //     throw msg_fmt(
  //         "TLS: peer certificate was not issued by a trusted authority");
  //   } else if (status & GNUTLS_CERT_INSECURE_ALGORITHM) {
  //     log_v2::tls()->error(
  //         "TLS: peer certificate is using an insecure algorithm that cannot
  //         be " "trusted");
  //     throw msg_fmt(
  //         "TLS: peer certificate is using an insecure algorithm that cannot
  //         be " "trusted");
  //   }
  // }
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  @brief Clean the params instance.
 *
 *  All allocated ressources will be released.
 */
void params::_clean() {
  // if (_init) {
  //   if (_cert.empty() || _key.empty()) {
  //     if (CLIENT == _type)
  //       gnutls_anon_free_client_credentials(_cred.client);
  //     else
  //       gnutls_anon_free_server_credentials(_cred.server);
  //   } else
  //     gnutls_certificate_free_credentials(_cred.cert);
  //   _init = false;
  // }
}

/**
 *  Initialize anonymous credentials.
 */
void params::_init_anonymous() {}
