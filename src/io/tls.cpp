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
#include <cstdlib>
#include <gnutls/gnutls.h>
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
 *  \brief TLSStream copy constructor.
 *
 *  TLSStreams are not copiable, that's why the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] tls_stream Unused.
 */
TLSStream::TLSStream(const TLSStream& tls_stream) : Stream(tls_stream)
{
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  TLSStreams are not copiable, that's why the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] tls_stream Unused.
 *
 *  \return *this
 */
TLSStream& TLSStream::operator=(const TLSStream& tls_stream)
{
  (void)tls_stream;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief TLSStream constructor.
 *
 *  When building the TLSStream, you need to provide a stream that will be used
 *  to transport encrypted data and a TLS session, providing informations on
 *  the kind of encryption to use. Upon completion of this constructor, the
 *  TLSStream object is considered to be the owner of the given objects, which
 *  means that the TLSStream object is responsible for their destruction.
 *
 *  \param[in] lower   The stream object that will transport encrypted data.
 *  \param[in] session TLS session, providing informations on the encryption
 *                     that should be used.
 */
TLSStream::TLSStream(Stream* lower, gnutls_session_t* session) throw ()
  : lower_(lower), session_(session) {}

/**
 *  \brief TLSStream destructor.
 *
 *  The destructor will release all acquired ressources that haven't been
 *  released yet.
 */
TLSStream::~TLSStream()
{
  this->Close();
}

/**
 *  \brief Close the TLS stream.
 *
 *  This method will shutdown the TLS session and close the underlying stream,
 *  releasing all acquired ressources.
 */
void TLSStream::Close()
{
  if (this->session_)
    {
      gnutls_bye(*this->session_, GNUTLS_SHUT_RDWR);
      gnutls_deinit(*this->session_);
      delete (this->session_);
      this->session_ = NULL;
    }
  if (this->lower_)
    {
      this->lower_->Close();
      delete (this->lower_);
      this->lower_ = NULL;
    }
  return ;
}

/**
 *  \brief Receive data from the TLS session.
 *
 *  Receive at most size bytes from the network stream and store them in
 *  buffer. The number of bytes read is then returned. This number can be less
 *  than size. In case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[out] buffer Buffer on which to store received data.
 *  \param[in]  size   Maximum number of bytes to read.
 *
 *  \return Number of bytes read from the network stream. 0 if the session has
 *          been shut down.
 */
unsigned int TLSStream::Receive(char* buffer, unsigned int size)
{
  int ret;

  ret = gnutls_record_recv(*this->session_, buffer, size);
  if (ret < 0)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  return ((unsigned int)ret);
}

/**
 *  \brief Send data across the TLS session.
 *
 *  Send at most size bytes from the buffer. The number of bytes actually sent
 *  is returned. This number can be less than size. In case of error, a
 *  CentreonBroker::Exception is thrown.
 *
 *  \param[in] buffer Data to send.
 *  \param[in] size   Maximum number of bytes to send.
 *
 *  \return Number of bytes actually sent through the TLS session. 0 if the
 *          connection has been shut down.
 */
unsigned int TLSStream::Send(const char* buffer, unsigned int size)
{
  int ret;

  ret = gnutls_record_send(*this->session_, buffer, size);
  if (ret < 0)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  return ((unsigned int)ret);
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
static const unsigned char dh_params_2048[] =
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
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief TLSAcceptor copy constructor.
 *
 *  As TLSAcceptors are not copiable, the copy constructor is declared private.
 *  Any attempt to use it will result in a call to abort().
 *
 *  \param[in] tls_acceptor Unused.
 */
TLSAcceptor::TLSAcceptor(const TLSAcceptor& tls_acceptor)
  : Acceptor(tls_acceptor)
{
  (void)tls_acceptor;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  As TLSAcceptors are not copiable, the operator= method is declared private.
 *  Any attempt to use it will result in a call to abort().
 *
 *  \param[in] tls_acceptor Unused.
 *
 *  \return *this
 */
TLSAcceptor& TLSAcceptor::operator=(const TLSAcceptor& tls_acceptor)
{
  (void)tls_acceptor;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief TLSAcceptor default constructor.
 *
 *  This constructor will initialize internal data, especially Diffie-Hellman
 *  parameters. In case of error, a CentreonBroker::Exception is thrown.
 */
TLSAcceptor::TLSAcceptor()
  : cert_based_(false),
    check_cert_(false),
    compression_(false),
    cred_init_(false),
    lower_(NULL)
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
 *  \brief TLSAcceptor destructor.
 *
 *  Release all acquired ressources like Diffie-Hellman parameters,
 *  credentials, ... and close the underlying acceptor if it has not already
 *  been closed.
 */
TLSAcceptor::~TLSAcceptor()
{
  this->Close();
  gnutls_dh_params_deinit(this->dh_params_);
}

/**
 *  \brief Try to accept a new connection.
 *
 *  Wait for an incoming client through the underlying acceptor, perform TLS
 *  checks (if configured to do so) and return a TLS encrypted stream. In case
 *  of error, a CentreonBroker::Exception is thrown.
 *
 *  \return A TLS-encrypted stream (namely a TLSStream object).
 *
 *  \see TLSStream
 */
Stream* TLSAcceptor::Accept()
{
  Stream* lower;
  gnutls_session_t* session;
  TLSStream* stream;

  /*
  ** The process of accepting a TLS client is pretty straight-forward. Just
  ** follow the comments the have an overview of performed operations.
  */

  // First accept a client from the lower layer.
  lower = this->lower_->Accept();
  session = NULL;
  stream = NULL;
  if (lower)
    {
      try
	{
	  int ret;

	  // Initialize the TLS session
	  session = new (gnutls_session_t);
	  ret = gnutls_init(session, GNUTLS_SERVER);
	  if (ret != GNUTLS_E_SUCCESS)
	    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));

	  // Set the encryption method (normal ciphers with anonymous
	  // Diffie-Hellman and optionnally compresion).
	  ret = gnutls_priority_set_direct(*session,
                                           (this->compression_
                                            ? "NORMAL:+ANON-DH:%COMPAT"
                                            : "NORMAL:+ANON-DH:+COMP-DEFLATE:%COMPAT"),
                                           NULL);
	  if (ret != GNUTLS_E_SUCCESS)
	    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));

	  // Set the anonymous credentials to use for encryption ...
	  if (!this->cert_based_)
            ret = gnutls_credentials_set(*session,
                                         GNUTLS_CRD_ANON,
                                         this->cred_.anon);
	  // ... or the certificates.
	  else
	    {
              ret = gnutls_credentials_set(*session,
                                           GNUTLS_CRD_CERTIFICATE,
                                           this->cred_.cert);
	      gnutls_certificate_server_set_request(*session,
						    GNUTLS_CERT_REQUIRE);
	    }
          if (ret != GNUTLS_E_SUCCESS)
            throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));

	  // Bind the TLS session with the stream from the lower layer.
          gnutls_transport_set_lowat(*session, 0);
          gnutls_transport_set_pull_function(*session, PullHelper);
          gnutls_transport_set_push_function(*session, PushHelper);
          gnutls_transport_set_ptr(*session, lower);

	  // Perform the TLS handshake.
	  do
	    {
	      ret = gnutls_handshake(*session);
	    } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
	  if (ret != GNUTLS_E_SUCCESS)
	    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));

	  // If set, check if the peer's certificate is valid.
	  if (this->check_cert_)
	    {
	      unsigned int status;

	      ret = gnutls_certificate_verify_peers2(*session, &status);
	      if (ret != GNUTLS_E_SUCCESS)
		throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
	      if (status & GNUTLS_CERT_INVALID)
		throw (CentreonBroker::Exception(GNUTLS_CERT_INVALID,
                  "The certificate is not signed by one of the known " \
                  "authorities, or the signature is invalid."));
	      else if (status & GNUTLS_CERT_REVOKED)
		throw (CentreonBroker::Exception(GNUTLS_CERT_REVOKED,
                  "The certificate has been revoked by its CA."));
	      else if (status & GNUTLS_CERT_SIGNER_NOT_FOUND)
		throw (CentreonBroker::Exception(GNUTLS_CERT_SIGNER_NOT_FOUND,
                  "The issuer is not in the trusted certificates list."));
	      else if (status & GNUTLS_CERT_INSECURE_ALGORITHM)
		throw (CentreonBroker::Exception(GNUTLS_CERT_INSECURE_ALGORITHM,
                  "The certificate was signed using an insecure algorithm " \
		  "such as MD2 or MD5. These algorithms have been broken "  \
                  "and should not be trusted."));
	    }
	  stream = new TLSStream(lower, session);
	}
      catch (...)
	{
	  if (session)
	    {
	      gnutls_deinit(*session);
	      delete (session);
	    }
	  lower->Close();
	  delete (lower);
	  throw ;
	}
    }
  return (stream);
}

/**
 *  \brief Close the acceptor.
 *
 *  Release all acquired ressources and close the underlying Acceptor.
 */
void TLSAcceptor::Close()
{
  if (this->lower_)
    {
      this->lower_->Close();
      delete (this->lower_);
      this->lower_ = NULL;
    }
  if (this->cred_init_)
    {
      if (this->cert_based_)
	gnutls_certificate_free_credentials(this->cred_.cert);
      else
	gnutls_anon_free_server_credentials(this->cred_.anon);
      this->cred_init_ = false;
    }
  this->cert_based_ = false;
  this->check_cert_ = false;
  this->compression_ = false;
  return ;
}

/**
 *  \brief Get the TLSAcceptor ready.
 *
 *  Initialize late TLS-related objects. From now on, users can call Accept()
 *  to wait for new clients. Upon a successful return of this method, the
 *  TLSAcceptor object is considered owning the Acceptor object provided, which
 *  means that the TLSAcceptor object will handle its destruction.
 *
 *  In case of error, a CentreonBroker::Exception is thrown.
 *
 *  \param[in] lower The underlying Acceptor.
 */
void TLSAcceptor::Listen(Acceptor* lower) throw (CentreonBroker::Exception)
{
  if (this->cert_based_)
    gnutls_certificate_set_dh_params(this->cred_.cert, this->dh_params_);
  else
    {
      int ret;

      ret = gnutls_anon_allocate_server_credentials(&this->cred_.anon);
      if (ret != GNUTLS_E_SUCCESS)
	throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
      gnutls_anon_set_server_dh_params(this->cred_.anon, this->dh_params_);
      this->cred_init_ = true;
    }
  this->lower_ = lower;
  return ;
}

/**
 *  \brief Set certificates to use for connection encryption.
 *
 *  TLSAcceptor provides two encryption mode : anonymous and certificate-based.
 *  If you want to use certificates for encryption, call this function with the
 *  name of the PEM-encoded public certificate (cert) and the private key
 *  (key). In case there was an error loading this pair, a
 *  CentreonBroker::Exception is thrown.
 *
 *  \param[in] cert The path to the PEM-encoded public certificate.
 *  \param[in] key  The path to the PEM-encoded private key.
 */
void TLSAcceptor::SetCert(const char* cert, const char* key)
  throw (CentreonBroker::Exception)
{
  int ret;

  ret = gnutls_certificate_allocate_credentials(&this->cred_.cert);
  if (ret != GNUTLS_E_SUCCESS)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  ret = gnutls_certificate_set_x509_key_file(this->cred_.cert,
                                             cert,
                                             key,
                                             GNUTLS_X509_FMT_PEM);
  if (ret != GNUTLS_E_SUCCESS)
    {
      gnutls_certificate_free_credentials(this->cred_.cert);
      throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
    }
  this->cert_based_ = true;
  this->cred_init_ = true;
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
 *  \param[in] compress_stream True if the stream should be compressed, false
 *                             otherwise.
 */
void TLSAcceptor::SetCompression(bool compress_stream) throw ()
{
  this->compression_ = compress_stream;
  return ;
}

/**
 *  \brief Set the trusted CA certificate.
 *
 *  If this parameter is set, certificate checking will be performed on the
 *  connection against this CA certificate. The SetCert method should have been
 *  called before. In case of error, a CentreonBroker::Exception will be
 *  thrown.
 *
 *  \param[in] ca_cert The path to the PEM-encoded public certificate of the
 *                     trusted Certificate Authority.
 */
void TLSAcceptor::SetTrustedCA(const char* ca_cert)
  throw (CentreonBroker::Exception)
{
  int ret;

  if (!this->cert_based_)
    throw (CentreonBroker::Exception(0, "Certificate used for encryption " \
                                        "should be set before the trusted " \
                                        "Certificate Authority's."));
  ret = gnutls_certificate_set_x509_trust_file(this->cred_.cert,
                                               ca_cert,
                                               GNUTLS_X509_FMT_PEM);
  if (ret <= 0)
    throw (CentreonBroker::Exception(ret, gnutls_strerror(ret)));
  this->check_cert_ = true;
  return ;
}
