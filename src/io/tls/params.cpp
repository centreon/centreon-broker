/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <gnutls/gnutls.h>
#include <stdlib.h>        // for abort
#include "exception.h"
#include "io/tls/internal.h"
#include "io/tls/params.h"

using namespace IO::TLS;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Params copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] params Unused.
 */
Params::Params(const Params& params)
{
  (void)params;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Any call to this operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] params Unused.
 *
 *  \return *this.
 */
Params& Params::operator=(const Params& params)
{
  (void)params;
  assert(false);
  abort();
  return (*this);
}

/**
 *  \brief Clean the Params instance.
 *
 *  All allocated ressources will be released.
 *  \par Safety No throw guarantee.
 */
void Params::Clean()
{
  if (this->init_)
    {
      if (this->anonymous_)
        {
          if (CLIENT == this->type_)
            gnutls_anon_free_client_credentials(this->cred_.client);
          else
            gnutls_anon_free_server_credentials(this->cred_.server);
        }
      else
        gnutls_certificate_free_credentials(this->cred_.cert);
      this->init_ = false;
    }
  return ;
}

/**
 *  Initialize anonymous credentials.
 *
 *  \throw Exception Could not allocate anonymous credentials.
 */
void Params::InitAnonymous()
{
  int ret;

  this->anonymous_ = true;
  if (CLIENT == this->type_)
    ret = gnutls_anon_allocate_client_credentials(&this->cred_.client);
  else
    ret = gnutls_anon_allocate_server_credentials(&this->cred_.server);
  if (ret != GNUTLS_E_SUCCESS)
    throw (Exception(ret, gnutls_strerror(ret)));
  if (this->type_ != CLIENT)
    gnutls_anon_set_server_dh_params(this->cred_.server, dh_params);
  this->init_ = true;
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
 *  \param[in] type Either CLIENT or SERVER, depending on connection
 *                  initialization. This cannot be modified after construction.
 *
 *  \throw Exception Initialization failed.
 */
Params::Params(Params::ConnectionType type)
  : anonymous_(false),
    check_cert_(false),
    compress_(false),
    init_(false),
    type_(type)
{
  this->InitAnonymous();
}

/**
 *  Apply parameters to a GNU TLS session object.
 *
 *  \param[out] session Object on which parameters will be applied.
 *
 *  \throw Exception Error occured while applying parameters to the session.
 */
void Params::Apply(gnutls_session_t session)
{
  int ret;

  // Set the encryption method (normal ciphers with anonymous Diffie-Hellman
  // and optionnally compression).
  ret = gnutls_priority_set_direct(session,
                                   (this->compress_
                                    ? "NORMAL:+ANON-DH:%COMPAT"
                                    : "NORMAL:+ANON-DH:+COMP-DEFLATE:%COMPAT"),
                                   NULL);
  if (ret != GNUTLS_E_SUCCESS)
    throw (Exception(ret, gnutls_strerror(ret)));

  // Set proper credentials
  if (this->anonymous_)
    {
      if (CLIENT == this->type_)
        ret = gnutls_credentials_set(session,
                                     GNUTLS_CRD_ANON,
                                     this->cred_.client);
      else
        ret = gnutls_credentials_set(session,
                                     GNUTLS_CRD_ANON,
                                     this->cred_.server);
    }
  else
    {
      ret = gnutls_credentials_set(session,
                                   GNUTLS_CRD_CERTIFICATE,
                                   this->cred_.cert);
      gnutls_certificate_server_set_request(session, GNUTLS_CERT_REQUIRE);
    }
  if (ret != GNUTLS_E_SUCCESS)
    throw (Exception(ret, gnutls_strerror(ret)));

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Params destructor.
 */
Params::~Params()
{
  this->Clean();
}

/**
 *  \brief Check if the peer's certificate is valid.
 *
 *  Check if the certificate invalid or revoked or untrusted or insecure. In
 *  those case, the connection should not be trusted. If no certificate is used
 *  for this connection or no trusted CA has been set, the method will return
 *  false.
 *
 *  \param[in] session Session on which checks will be performed.
 *
 *  \return false if the certificate is valid, true otherwise.
 *
 *  \throw Exception Unable to perform certificate check.
 */
bool Params::CheckCert(gnutls_session_t session)
{
  bool invalid;

  if (this->check_cert_)
    {
      int ret;
      unsigned int status;

      ret = gnutls_certificate_verify_peers2(session, &status);
      if (ret != GNUTLS_E_SUCCESS)
        throw (Exception(ret, gnutls_strerror(ret)));
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
 *  \brief Reset parameters to their default values.
 *
 *  Parameters are changed back to the default anonymous mode without
 *  compression.
 *
 *  \throw Exception Reset failed.
 */
void Params::Reset()
{
  this->Clean();
  this->InitAnonymous();
  return ;
}

/**
 *  \brief Set certificates to use for connection encryption.
 *
 *  IO::TLS::Acceptor provides two encryption mode : anonymous and
 *  certificate-based. If you want to use certificates for encryption, call
 *  this function with the name of the PEM-encoded public certificate (cert)
 *  and the private key (key).
 *
 *  \param[in] cert The path to the PEM-encoded public certificate.
 *  \param[in] key  The path to the PEM-encoded private key.
 *
 *  \throw Exception Failed to load certificate files.
 */
void Params::SetCert(const std::string& cert, const std::string& key)
{
  int ret;

  // Initialize credentials if necessary.
  if (this->anonymous_)
    {
      this->Clean();
      this->anonymous_ = false;
      ret = gnutls_certificate_allocate_credentials(&this->cred_.cert);
      if (ret != GNUTLS_E_SUCCESS)
        throw (Exception(ret, gnutls_strerror(ret)));
      gnutls_certificate_set_dh_params(this->cred_.cert, dh_params);
      this->check_cert_ = false;
      this->init_ = true;
    }

  // Load certificate files.
  ret = gnutls_certificate_set_x509_key_file(this->cred_.cert,
                                             cert.c_str(),
                                             key.c_str(),
                                             GNUTLS_X509_FMT_PEM);
  if (ret != GNUTLS_E_SUCCESS)
    throw (Exception(ret, gnutls_strerror(ret)));

  return ;
}

/**
 *  \brief Set the compression mode (on/off).
 *
 *  Determines whether or not the encrypted stream should also be compressed
 *  using the Deflate algorithm. This kind of compression usually works well on
 *  text or other compressible data. The compression algorithm, may be useful
 *  in high bandwidth TLS tunnels, and in cases where network usage has to be
 *  minimized. As a drawback, compression increases latency.
 *
 *  \param[in] compress true if the stream should be compressed, false
 *                      otherwise.
 */
void Params::SetCompression(bool compress)
{
  this->compress_ = compress;
  return ;
}

/**
 *  \brief Set the trusted CA certificate.
 *
 *  If this parameter is set, certificate checking will be performed on the
 *  connection against this CA certificate. The SetCert method should have been
 *  called before.
 *
 *  \param[in] ca_cert The path to the PEM-encoded public certificate of the
 *                     trusted Certificate Authority.
 */
void Params::SetTrustedCA(const std::string& ca_cert)
{
  int ret;

  // SetTrustedCA() _has to_ be called _after_ SetCert().
  if (this->anonymous_)
    throw (Exception(0, "Certificate used for encryption should be set" \
                        " before the trusted Certificate Authority's."));

  // Load certificate.
  ret = gnutls_certificate_set_x509_trust_file(this->cred_.cert,
                                               ca_cert.c_str(),
                                               GNUTLS_X509_FMT_PEM);
  if (ret <= 0)
    throw (Exception(ret, gnutls_strerror(ret)));

  // Certificate checking has to be performed.
  this->check_cert_ = true;

  return ;
}
