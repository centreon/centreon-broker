/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_TLS_PARAMS_HH
#define CCB_TLS_PARAMS_HH

#include <gnutls/gnutls.h>

#include <string>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tls {
/**
 *  @class params params.hh "com/centreon/broker/tls/params.hh"
 *  @brief Configure parameters of a TLS connection (either incoming
 *         or outgoing).
 *
 *  params is used by the TLS acceptor and the TLS connector classes
 *  to configure the future TLS connections.
 */
class params {
 public:
  enum connection_type { CLIENT = 1, SERVER };

 private:
  std::string _ca;
  std::string _cert;
  std::string _tls_hostname;
  bool _compress;
  union {
    gnutls_certificate_credentials_t cert;
    gnutls_anon_client_credentials_t client;
    gnutls_anon_server_credentials_t server;
  } _cred;
  bool _init;
  std::string _key;
  connection_type _type;

  void _clean();
  void _init_anonymous();

 public:
  params(connection_type type);
  params(params const& p) = delete;
  params& operator=(params const& p) = delete;
  virtual ~params();
  void apply(gnutls_session_t session);
  void load();
  void reset();
  void set_cert(std::string const& cert, std::string const& key);
  void set_compression(bool compress = false);
  void set_trusted_ca(std::string const& ca_cert);
  void set_tls_hostname(std::string const& tls_hostname);
  void validate_cert(gnutls_session_t session);
};
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_PARAMS_HH
