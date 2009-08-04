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
# include "exception.h"
# include "io/io.h"

namespace               CentreonBroker
{
  namespace             IO
  {
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
      int                Receive(char* buffer, int size);
      int                Send(const char* buffer, int size);
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
    class                TLSAcceptor : public Acceptor
    {
     private:
      bool               cert_based_;
      bool               check_cert_;
      bool               compression_;
      union
      {
	gnutls_anon_server_credentials_t anon;
	gnutls_certificate_credentials_t cert;
      }                  cred_;
      bool               cred_init_;
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
      void               SetCert(const char* cert, const char* key)
                           throw (CentreonBroker::Exception);
      void               SetCompression(bool compress_stream = true)
                           throw ();
      void               SetTrustedCA(const char* ca_cert)
                           throw (CentreonBroker::Exception);
    };
  }
}

#endif /* !IO_TLS_H_ */
