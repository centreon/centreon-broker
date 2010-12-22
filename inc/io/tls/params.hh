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

#ifndef IO_TLS_PARAMS_HH_
# define IO_TLS_PARAMS_HH_

# include <gnutls/gnutls.h>
# include <string>

namespace             io {
  namespace           tls {
    /**
     *  @class params params.hh "io/tls/params.hh"
     *  @brief Configure parameters of a TLS connection (either incoming
     *         or outgoing).
     *
     *  params is used by the io::tls::acceptor and the
     *  io::tls::connector classes to configure the future TLS
     *  connections.
     */
    class             params {
     public:
      enum            connection_type {
        CLIENT = 1,
        SERVER
      };

     private:
      bool            _anonymous;
      bool            _check_cert;
      bool            _compress;
      union {
	gnutls_certificate_credentials_t cert;
	gnutls_anon_client_credentials_t client;
	gnutls_anon_server_credentials_t server;
      }               _cred;
      bool            _init;
      connection_type _type;
                      params(params const& p);
      params&         operator=(params const& p);
      void            _clean();
      void            _init_anonymous();

     protected:
                      params(connection_type type);
      void            apply(gnutls_session_t session);

     public:
      virtual         ~params();
      bool            check_cert(gnutls_session_t session);
      void            reset();
      void            set_cert(std::string const& cert,
                        std::string const& key);
      void            set_compression(bool compress = true);
      void            set_trusted_ca(std::string const& ca_cert);
    };
  }
}

#endif /* !IO_TLS_PARAMS_HH_ */
