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

#include <cassert>
#include <gnutls.h>
#include "exception.h"
#include "io/tls.h"

using namespace CentreonBroker::IO;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                 TLSStream                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  The following static function is used to receive data from the lower layer
 *  and give it to TLS for decoding.
 */
static ssize_t PullHelper(gnutls_transport_ptr_t ptr, void* data, size_t size)
{
  return (static_cast<Stream*>(ptr)->Receive(static_cast<char*>(data), size));
}

/**
 *  The following static function is used to send data from TLS to the lower
 *  layer.
 */
static ssize_t PushHelper(gnutls_transport_ptr_t ptr,
                          const void* data,
                          size_t size)
{
  return (static_cast<Stream*>(ptr)->Send(static_cast<const char*>(data),
                                          size));
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  TLSStream default constructor.
 */
TLSStream::TLSStream(Stream* lower)
  : lower_(lower)
{
  int ret;

  ret = gnutls_init(&this->session_, GNUTLS_SERVER);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  gnutls_transport_set_lowat(this->session_, 0);
  gnutls_transport_set_pull_function(this->session_, PullHelper);
  gnutls_transport_set_push_function(this->session_, PushHelper);
  gnutls_transport_set_ptr(this->session_, this->lower_);
}

/**
 *  TLSStream copy constructor.
 */
TLSStream::TLSStream(const TLSStream& tls_stream) : Stream(tls_stream)
{
  assert(false);
}

/**
 *  TLSStream operator= overload.
 */
TLSStream& TLSStream::operator=(const TLSStream& tls_stream)
{
  (void)tls_stream;
  assert(false);
  return (*this);
}

/**
 *  Perform a TLS handshake.
 */
void TLSStream::Handshake()
{
  int ret;

  do
    {
      ret = gnutls_handshake(this->session_);
    } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  return ;
}

/**
 *  Verify if peer certificate is valid.
 */
void TLSStream::VerifyPeer()
{
  int ret;
  unsigned int status;

  ret = gnutls_certificate_verify_peers2(this->session_, &status);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  if (status & GNUTLS_CERT_INVALID)
    throw (CentreonBroker::Exception(GNUTLS_CERT_INVALID,
      "The certificate is not signed by one of the known authorities, or " \
      "the signature is invalid."));
  else if (status & GNUTLS_CERT_REVOKED)
    throw (CentreonBroker::Exception(GNUTLS_CERT_REVOKED,
      "The certificate has been revoked by its CA."));
  else if (status & GNUTLS_CERT_SIGNER_NOT_FOUND)
    throw (CentreonBroker::Exception(GNUTLS_CERT_SIGNER_NOT_FOUND,
      "The issuer is not in the trusted certificates list."));
  else if (status & GNUTLS_CERT_INSECURE_ALGORITHM)
    throw (CentreonBroker::Exception(GNUTLS_CERT_INSECURE_ALGORITHM,
      "The certificate was signed using an insecure algorithm such as MD2 " \
      "or MD5. These algorithms have been broken and should not be trusted."));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  TLSStream destructor.
 */
TLSStream::~TLSStream()
{
  this->Close();
  gnutls_deinit(this->session_);
}

/**
 *  Close all connection-related objects.
 */
void TLSStream::Close()
{
  if (this->lower_)
    {
      gnutls_bye(this->session_, GNUTLS_SHUT_RDWR);
      this->lower_->Close();
      delete (this->lower_);
      this->lower_ = NULL;
    }
  return ;
}

/**
 *  Return the lower layer.
 */
Stream* TLSStream::GetLower() const throw ()
{
  return (this->lower_);
}

/**
 *  Receive data.
 */
int TLSStream::Receive(char* buffer, int size)
{
  int ret;

  do
    {
      ret = gnutls_record_recv(this->session_, buffer, size);
    } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  return (ret);
}

/**
 *  Send data.
 */
int TLSStream::Send(const char* buffer, int size)
{
  int ret;

  do
    {
      ret = gnutls_record_send(this->session_, buffer, size);
    } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  return (ret);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               TLSAcceptor                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Those 2048-bits wide Diffie-Hellman parameters were generated the
 *  30/07/2009 on Ubuntu 9.04 x86 using OpenSSL 0.9.8g with generator 2.
 */
const unsigned char TLSAcceptor::dh_params_2048[] =
  "-----BEGIN DH PARAMETERS-----\n" \
  "MIIBCAKCAQEA93F3CN41kJooLbqcOdWHJPb+/zPV+mMs5Svb6PVH/XS3BK/tuuVu\n" \
  "r9okkOzGr07KLPiKf+3MJSgHs9N91wPG6JcMcRys3fH1Tszh1i1317tE54o+oLPv\n" \
  "jcs9P13lFlZm4gB7sjkR5If/ZtudoVwv7JS5WHIXrzew7iW+kT/QXCp+jkO1Vusc\n" \
  "mQHlq4Fqt/p7zxOHVc8GBttE6/vEYipm2pdym1kBy62Z6rZLowkukngI5uzdQvB4\n" \
  "Pmq5BmeRzGRClSkmRW4pUXiBac8SMAgMBl7cgAEaURR2D8Y4XltyXW51xzO1x1QM\n" \
  "bOl9nneRY2Y8X3FOR1+Mzt+x44F+cWtqIwIBAg==\n" \
  "-----END DH PARAMETERS-----\n";

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  TLSAcceptor default constructor.
 */
TLSAcceptor::TLSAcceptor()
  : compression_(false), cred_init_(false)
{
  const gnutls_datum_t dh_params =
    { const_cast<unsigned char*>(dh_params_2048), sizeof(dh_params_2048) };
  int ret;

  ret = gnutls_dh_params_init(&this->dh_params_);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  ret = gnutls_dh_params_import_pkcs3(this->dh_params_,
                                      &dh_params,
                                      GNUTLS_X509_FMT_PEM);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
}

/**
 *  TLSAcceptor destructor.
 */
TLSAcceptor::~TLSAcceptor()
{
  this->Close();
  gnutls_dh_params_deinit(this->dh_params_);
}

/**
 *  Try to accept a new connection.
 */
Stream* TLSAcceptor::Accept()
{
  Stream* lower;
  TLSStream* stream;

  lower = this->lower_->Accept();
  stream = NULL;
  if (lower)
    {
      try
	{
	  int ret;

	  stream = new TLSStream(lower);
	  if (this->cert_.empty() || this->key_.empty())
            ret = gnutls_credentials_set(stream->session_,
                                         GNUTLS_CRD_ANON,
                                         this->cred_.anon);
	  else
	    {
              ret = gnutls_credentials_set(stream->session_,
                                           GNUTLS_CRD_CERTIFICATE,
                                           this->cred_.cert);
	      gnutls_certificate_server_set_request(stream->session_,
						    GNUTLS_CERT_REQUIRE);
	    }
          if (ret != GNUTLS_E_SUCCESS)
            throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
	  ret = gnutls_priority_set_direct(stream->session_,
                                           (this->compression_
                                            ? "NORMAL:+ANON-DH"
                                            : "NORMAL:+ANON-DH:+COMP-DEFLATE"),
                                           NULL);
	  if (ret != GNUTLS_E_SUCCESS)
	    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
	  stream->Handshake();
	  if (!this->ca_cert_.empty())
	    stream->VerifyPeer();
	}
      catch (...)
	{
	  if (stream)
	    delete (stream);
	  else if (lower)
	    delete (lower);
	  throw ;
	}
    }
  return (stream);
}

/**
 *  Close the acceptor.
 */
void TLSAcceptor::Close()
{
  if (this->lower_)
    {
      this->lower_->Close();
      this->lower_ = NULL;
    }
  if (this->cred_init_)
    {
      if (!this->cert_.empty() && !this->key_.empty())
	gnutls_certificate_free_credentials(this->cred_.cert);
      else
	gnutls_anon_free_server_credentials(this->cred_.anon);
      this->cred_init_ = false;
    }
  return ;
}

/**
 *  Get the lower acceptor.
 */
Acceptor* TLSAcceptor::GetLower() const throw ()
{
  return (this->lower_);
}

/**
 *  Put the acceptor in listen mode.
 */
void TLSAcceptor::Listen()
{
  int ret;

  if (!this->cert_.empty() && !this->key_.empty())
    {
      ret = gnutls_certificate_allocate_credentials(&this->cred_.cert);
      if (ret != GNUTLS_E_SUCCESS)
	throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
      ret = gnutls_certificate_set_x509_key_file(this->cred_.cert,
                                                 this->cert_.c_str(),
                                                 this->key_.c_str(),
                                                 GNUTLS_X509_FMT_PEM);
      if (ret != GNUTLS_E_SUCCESS)
	{
	  gnutls_certificate_free_credentials(this->cred_.cert);
	  throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
	}
      ret = gnutls_certificate_set_x509_trust_file(this->cred_.cert,
                                                   this->ca_cert_.c_str(),
                                                   GNUTLS_X509_FMT_PEM);
      if (ret < 0)
	{
	  gnutls_certificate_free_credentials(this->cred_.cert);
	  throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
	}
      gnutls_certificate_set_dh_params(this->cred_.cert, this->dh_params_);
    }
  else
    {
      ret = gnutls_anon_allocate_server_credentials(&this->cred_.anon);
      if (ret != GNUTLS_E_SUCCESS)
	throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
      gnutls_anon_set_server_dh_params(this->cred_.anon, this->dh_params_);
    }
  this->cred_init_ = true;
  this->lower_->Listen();
  return ;
}

/**
 *  Set certificates to use for connection encryption.
 */
void TLSAcceptor::SetCert(const std::string& cert,
                          const std::string& key)
{
  this->cert_ = cert;
  this->key_ = key;
  return ;
}

/**
 *  Set the lower acceptor.
 */
void TLSAcceptor::SetLower(Acceptor* lower) throw ()
{
  this->lower_ = lower;
  return ;
}

/**
 *  Set the trusted CA certificate. If this parameter is set, certificate
 *  checking will be performed on the connection against this CA certificate.
 */
void TLSAcceptor::SetTrustedCA(const std::string& ca_cert)
{
  this->ca_cert_ = ca_cert;
  return ;
}
