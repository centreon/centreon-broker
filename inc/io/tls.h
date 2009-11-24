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

#ifndef IO_TLS_H_
# define IO_TLS_H_

# include <gnutls/gnutls.h>
# include <string>
# include "exception.h"
# include "io/io.h"

namespace                CentreonBroker
{
  namespace              IO
  {
    /**
     *  \class TLSParams tls.h "io/tls.h"
     *  \brief Configure parameters of a TLS connection (either incoming or
     *         outgoing).
     *
     *  TLSParams is used by the TLSAcceptor and TLSConnector classes to
     *  configure the future TLS connections.
     */
    class                TLSParams
    {
     public:
      enum               ConnectionType
      {
        CLIENT = 1,
        SERVER
      };

     private:
      bool               anonymous_;
      bool               check_cert_;
      bool               compress_;
      union
      {
	gnutls_certificate_credentials_t cert;
	gnutls_anon_client_credentials_t client;
	gnutls_anon_server_credentials_t server;
      }                  cred_;
      bool               init_;
      ConnectionType     type_;
                         TLSParams(const TLSParams& params);
      TLSParams&         operator=(const TLSParams& params);
      void               Clean() throw ();
      void               InitAnonymous() throw (CentreonBroker::Exception);

     protected:
                         TLSParams(ConnectionType type)
                           throw (CentreonBroker::Exception);
      void               Apply(gnutls_session_t session)
                           throw (CentreonBroker::Exception);

     public:
      virtual            ~TLSParams();
      bool               CheckCert(gnutls_session_t session)
                           throw (CentreonBroker::Exception);
      void               Reset() throw (CentreonBroker::Exception);
      void               SetCert(const std::string& cert,
                                 const std::string& key);
      void               SetCompression(bool compress = true);
      void               SetTrustedCA(const std::string& ca_cert)
                           throw (CentreonBroker::Exception);
    };

    /**
     *  \class TLSStream tls.h "io/tls.h"
     *  \brief TLS wrapper of an underlying stream.
     *
     *  The TLSStream class wraps a lower layer stream and provides encryption
     *  (and optionnally compression) over this stream. Those functionnality
     *  are provided using the GNU TLS library
     *  (http://www.gnu.org/software/gnutls). TLSStream can be used on every
     *  Stream object.
     */
    class                TLSStream : public Stream
    {
     private:
      Stream*            lower_;
      gnutls_session_t*  session_;
                         TLSStream(const TLSStream& tls_stream);
      TLSStream&         operator=(const TLSStream& tls_stream);

     public:
                         TLSStream(Stream* lower, gnutls_session_t* session)
                           throw ();
                         ~TLSStream();
      void               Close();
      unsigned int       Receive(void* buffer, unsigned int size);
      unsigned int       Send(const void* buffer, unsigned int size);
    };

    /**
     *  \class TLSAcceptor tls.h "io/tls.h"
     *  \brief Perform TLS verification on top of another acceptor.
     *
     *  Within the process of accepting an incoming client, the TLSAcceptor
     *  class will provide encryption to the lower stream. Using this class is
     *  really simple : build the object, set some properties and call Listen()
     *  with the lower acceptor. Then use it just like you'd use another
     *  Acceptor. Encryption will be automatically provided on the returned
     *  accepted streams.
     */
    class                TLSAcceptor : public Acceptor, public TLSParams
    {
     private:
      gnutls_dh_params_t dh_params_;
      Acceptor*          lower_;
                         TLSAcceptor(const TLSAcceptor& tls_acceptor);
      TLSAcceptor&       operator=(const TLSAcceptor& tls_acceptor);

     public:
                         TLSAcceptor();
                         ~TLSAcceptor();
      Stream*            Accept();
      void               Close();
      void               Listen(Acceptor* acceptor)
                           throw (CentreonBroker::Exception);
    };
  }
}

#endif /* !IO_TLS_H_ */
