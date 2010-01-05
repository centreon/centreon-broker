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

#ifndef IO_TLS_PARAMS_H_
# define IO_TLS_PARAMS_H_

# include <gnutls/gnutls.h>
# include <string>

namespace            IO
{
  namespace          TLS
  {
    /**
     *  \class Params params.h "io/tls/params.h"
     *  \brief Configure parameters of a TLS connection (either incoming or
     *         outgoing).
     *
     *  Params is used by the IO::TLS::Acceptor and the IO::TLS::Connector
     *  classes to configure the future TLS connections.
     */
    class            Params
    {
     public:
      enum           ConnectionType
      {
        CLIENT = 1,
        SERVER
      };

     private:
      bool           anonymous_;
      bool           check_cert_;
      bool           compress_;
      union
      {
	gnutls_certificate_credentials_t cert;
	gnutls_anon_client_credentials_t client;
	gnutls_anon_server_credentials_t server;
      }              cred_;
      bool           init_;
      ConnectionType type_;
                     Params(const Params& params);
      Params&        operator=(const Params& params);
      void           Clean();
      void           InitAnonymous();

     protected:
                     Params(ConnectionType type);
      void           Apply(gnutls_session_t session);

     public:
      virtual        ~Params();
      bool           CheckCert(gnutls_session_t session);
      void           Reset();
      void           SetCert(const std::string& cert, const std::string& key);
      void           SetCompression(bool compress = true);
      void           SetTrustedCA(const std::string& ca_cert);
    };
  }
}

#endif /* !IO_TLS_PARAMS_H_ */
