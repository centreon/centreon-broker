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

# include <gnutls.h>
# include "io/io.h"

namespace              CentreonBroker
{
  namespace            IO
  {
    class              TLSStream : public Stream
    {
      friend class     TLSAcceptor;

     private:
      Stream*          lower_;
      gnutls_session_t session_;
                       TLSStream(Stream* lower);
                       TLSStream(const TLSStream& tls_stream);
      TLSStream&       operator=(const TLSStream& tls_stream);
      void             Handshake();
      void             VerifyPeer();

     public:
                       ~TLSStream();
      void             Close();
      Stream*          GetLower() const throw ();
      int              Receive(char* buffer, int size);
      int              Send(const char* buffer, int size);
    };

    class              TLSAcceptor : public Acceptor
    {
     private:
      static const unsigned char
                         dh_params_2048[];
      std::string        ca_cert_;
      std::string        cert_;
      bool               compression_;
      union
      {
	gnutls_anon_server_credentials_t anon;
	gnutls_certificate_credentials_t cert;
      }                  cred_;
      bool               cred_init_;
      gnutls_dh_params_t dh_params_;
      std::string        key_;
      Acceptor*          lower_;
                         TLSAcceptor(const TLSAcceptor& tls_acceptor);
      TLSAcceptor&       operator=(const TLSAcceptor& tls_acceptor);

     public:
                         TLSAcceptor();
                         ~TLSAcceptor();
      Stream*            Accept();
      void               Close();
      Acceptor*          GetLower() const throw ();
      void               Listen();
      void               SetCert(const std::string& cert,
                                 const std::string& key);
      void               SetCompression(bool compress_stream = true);
      void               SetLower(Acceptor* lower) throw ();
      void               SetTrustedCA(const std::string& ca_cert);
    };
  }
}

#endif /* !IO_TLS_H_ */
