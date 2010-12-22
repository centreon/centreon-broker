/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <gnutls/gnutls.h>
#include <stdlib.h>
#include "exceptions/retval.hh"
#include "io/tls/internal.hh"
#include "io/tls/params.hh"

using namespace io::tls;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] p Unused.
 */
params::params(params const& p) {
  (void)p;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this operator will result in a call to abort().
 *
 *  @param[in] p Unused.
 *
 *  @return This object.
 */
params& params::operator=(params const& p) {
  (void)p;
  assert(false);
  abort();
  return (*this);
}

/**
 *  @brief Clean the params instance.
 *
 *  All allocated ressources will be released.
 */
void params::_clean() {
  if (_init) {
    if (_anonymous) {
      if (CLIENT == _type)
        gnutls_anon_free_client_credentials(_cred.client);
      else
        gnutls_anon_free_server_credentials(_cred.server);
    }
    else
      gnutls_certificate_free_credentials(_cred.cert);
    _init = false;
  }
  return ;
}

/**
 *  Initialize anonymous credentials.
 */
void params::_init_anonymous() {
  int ret;

  _anonymous = true;
  if (CLIENT == _type)
    ret = gnutls_anon_allocate_client_credentials(&_cred.client);
  else
    ret = gnutls_anon_allocate_server_credentials(&_cred.server);
  if (ret != GNUTLS_E_SUCCESS)
    throw (exceptions::retval(ret) << "TLS anonymous credentials initialization failed: "
                                   << gnutls_strerror(ret));
  if (_type != CLIENT)
    gnutls_anon_set_server_dh_params(_cred.server, dh_params);
  _init = true;
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
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
  : _anonymous(false),
    _check_cert(false),
    _compress(false),
    _init(false),
    _type(type) {
  _init_anonymous();
}

/**
 *  Apply parameters to a GNU TLS session object.
 *
 *  @param[out] session Object on which parameters will be applied.
 */
void params::apply(gnutls_session_t session) {
  int ret;

  // Set the encryption method (normal ciphers with anonymous Diffie-Hellman
  // and optionnally compression).
  ret = gnutls_priority_set_direct(session,
                                   (_compress
                                    ? "NORMAL:+ANON-DH:%COMPAT"
                                    : "NORMAL:+ANON-DH:+COMP-DEFLATE:%COMPAT"),
                                   NULL);
  if (ret != GNUTLS_E_SUCCESS)
    throw (exceptions::retval(ret) << "tls::params::apply: "
                                   << gnutls_strerror(ret));

  // Set proper credentials
  if (_anonymous) {
    if (CLIENT == _type)
      ret = gnutls_credentials_set(session,
                                   GNUTLS_CRD_ANON,
                                   _cred.client);
    else
      ret = gnutls_credentials_set(session,
                                   GNUTLS_CRD_ANON,
                                   _cred.server);
  }
  else {
    ret = gnutls_credentials_set(session,
                                 GNUTLS_CRD_CERTIFICATE,
                                 _cred.cert);
    gnutls_certificate_server_set_request(session, GNUTLS_CERT_REQUIRE);
  }
  if (ret != GNUTLS_E_SUCCESS)
    throw (exceptions::retval(ret) << "TLS credentials failed: "
                                   << gnutls_strerror(ret));

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
params::~params() {
  _clean();
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
 *
 *  @return false if the certificate is valid, true otherwise.
 */
bool params::check_cert(gnutls_session_t session) {
  bool invalid;

  if (_check_cert) {
    int ret;
    unsigned int status;

    ret = gnutls_certificate_verify_peers2(session, &status);
    if (ret != GNUTLS_E_SUCCESS)
      throw (exceptions::retval(ret) << "TLS certificate verification failed: "
                                     << gnutls_strerror(ret));
    invalid = status & (GNUTLS_CERT_INVALID
                        | GNUTLS_CERT_REVOKED
                        | GNUTLS_CERT_SIGNER_NOT_FOUND
                        | GNUTLS_CERT_INSECURE_ALGORITHM);
  }
  else
    invalid = false;
  return (invalid);
}

/**
 *  @brief Reset parameters to their default values.
 *
 *  Parameters are changed back to the default anonymous mode without
 *  compression.
 */
void params::reset() {
  _clean();
  _init_anonymous();
  return ;
}

/**
 *  @brief Set certificates to use for connection encryption.
 *
 *  io::tls::acceptor provides two encryption mode : anonymous and
 *  certificate-based. If you want to use certificates for encryption,
 *  call this function with the name of the PEM-encoded public
 *  certificate (cert) and the private key (key).
 *
 *  @param[in] cert The path to the PEM-encoded public certificate.
 *  @param[in] key  The path to the PEM-encoded private key.
 */
void params::set_cert(std::string const& cert, std::string const& key) {
  int ret;

  // Initialize credentials if necessary.
  if (_anonymous) {
    _clean();
    _anonymous = false;
    ret = gnutls_certificate_allocate_credentials(&_cred.cert);
    if (ret != GNUTLS_E_SUCCESS)
      throw (exceptions::retval(ret) << "could not allocate TLS credentials: "
                                     << gnutls_strerror(ret));
    gnutls_certificate_set_dh_params(_cred.cert, dh_params);
    _check_cert = false;
    _init = true;
  }

  // Load certificate files.
  ret = gnutls_certificate_set_x509_key_file(_cred.cert,
                                             cert.c_str(),
                                             key.c_str(),
                                             GNUTLS_X509_FMT_PEM);
  if (ret != GNUTLS_E_SUCCESS)
    throw (exceptions::retval(ret) << "could not load TLS certificate: "
                                   << gnutls_strerror(ret));

  return ;
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
  return ;
}

/**
 *  @brief Set the trusted CA certificate.
 *
 *  If this parameter is set, certificate checking will be performed on
 *  the connection against this CA certificate. The SetCert method
 *  should have been called before.
 *
 *  @param[in] ca_cert The path to the PEM-encoded public certificate of
 *                     the trusted Certificate Authority.
 */
void params::set_trusted_ca(std::string const& ca_cert) {
  int ret;

  // set_trusted_ca() _has to_ be called _after_ set-cert().
  if (_anonymous)
    throw (exceptions::basic() << "certificate used for encryption should be set" \
                                  " before the trusted Certificate Authority's");

  // Load certificate.
  ret = gnutls_certificate_set_x509_trust_file(_cred.cert,
                                               ca_cert.c_str(),
                                               GNUTLS_X509_FMT_PEM);
  if (ret <= 0)
    throw (exceptions::retval(ret) << "could not load TLS certificate: "
                                   << gnutls_strerror(ret));

  // Certificate checking has to be performed.
  _check_cert = true;

  return ;
}
