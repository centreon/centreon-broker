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

#include "com/centreon/broker/tls/connector.hh"

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/params.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;
using namespace com::centreon::exceptions;

/**
 *  Default constructor
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
connector::connector(std::string const& cert,
                     std::string const& key,
                     std::string const& ca,
                     std::string const& tls_hostname)
    : io::endpoint(false),
      _ca(ca),
      _cert(cert),
      _key(key),
      _tls_hostname(tls_hostname) {}

/**
 *  Connect to the remote TLS peer.
 *
 *  @return New connected stream.
 */
std::unique_ptr<io::stream> connector::open() {
  // First connect the lower layer.
  std::unique_ptr<io::stream> lower(_from->open());
  if (lower)
    return open(std::move(lower));
  return nullptr;
}

/**
 *  Overload of open, using base stream.
 *
 *  @param[in] lower Open stream.
 *
 *  @return Encrypted stream.
 */
std::unique_ptr<io::stream> connector::open(std::shared_ptr<io::stream> lower) {
  std::unique_ptr<io::stream> u;
  if (lower) {
    try {
      SSL_CTX* c_ctx = SSL_CTX_new(TLS_client_method());

      SSL_CTX_set_security_level(c_ctx, 0);

      SSL_CTX_set_ecdh_auto(c_ctx, 1);

      if (!SSL_CTX_set_cipher_list(c_ctx, "aNULL")) {
        log_v2::tls()->error("new connector cipher list error");
      }

      SSL* c_ssl = SSL_new(c_ctx);

      BIO* c_ssl_bio = nullptr;
      BIO *client = nullptr, *client_io = nullptr;
      size_t bufsiz = 2048; /* small buffer for testing */

      if (!BIO_new_bio_pair(&client, bufsiz, &client_io, bufsiz))
        log_v2::tls()->error("new crash ");

      c_ssl_bio = BIO_new(BIO_f_ssl());
      if (!c_ssl_bio)
        log_v2::tls()->error("new connector ssl bio error");
      SSL_set_connect_state(c_ssl);
      SSL_set_bio(c_ssl, client, client);
      (void)BIO_set_ssl(c_ssl_bio, c_ssl, BIO_NOCLOSE);
      // Create stream object.
      log_v2::tls()->error("tls juste avant le stream connector");
      u.reset(new stream(c_ssl, c_ssl_bio, client_io));
    } catch (...) {
      // delete c_ssl;
      throw;
    }
    u->set_substream(lower);
  }

  return u;
}
