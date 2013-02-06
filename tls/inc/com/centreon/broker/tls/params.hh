/*
** Copyright 2009-2013 Merethis
**
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
*/

#ifndef CCB_TLS_PARAMS_HH
#  define CCB_TLS_PARAMS_HH

#  include <gnutls/gnutls.h>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           tls {
  /**
   *  @class params params.hh "com/centreon/broker/tls/params.hh"
   *  @brief Configure parameters of a TLS connection (either incoming
   *         or outgoing).
   *
   *  params is used by the TLS acceptor and the TLS connector classes
   *  to configure the future TLS connections.
   */
  class             params {
  public:
    enum            connection_type {
      CLIENT = 1,
      SERVER
    };

  public:
                    params(connection_type type);
    virtual         ~params();
    void            apply(gnutls_session_t session);
    void            load();
    void            reset();
    void            set_cert(
                      std::string const& cert,
                      std::string const& key);
    void            set_compression(bool compress = false);
    void            set_trusted_ca(std::string const& ca_cert);
    void            validate_cert(gnutls_session_t session);

  private:
                    params(params const& p);
    params&         operator=(params const& p);
    void            _clean();
    void            _init_anonymous();

    std::string     _ca;
    std::string     _cert;
    bool            _compress;
    union {
      gnutls_certificate_credentials_t cert;
      gnutls_anon_client_credentials_t client;
      gnutls_anon_server_credentials_t server;
    }               _cred;
    bool            _init;
    std::string     _key;
    connection_type _type;
  };
}

CCB_END()

#endif // !CCB_TLS_PARAMS_HH
