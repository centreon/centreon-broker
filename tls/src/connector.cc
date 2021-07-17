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
connector::connector(std::string cert,
                     std::string key,
                     std::string ca,
                     std::string tls_hostname)
    : io::endpoint(false),
      _ca(std::move(ca)),
      _cert(std::move(cert)),
      _key(std::move(key)),
      _tls_hostname(std::move(tls_hostname)) {}

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
  std::unique_ptr<stream> u;
  if (lower) {
    try {
      SSL* c_ssl = SSL_new(tls::ctx);
      if (c_ssl == nullptr)
        throw msg_fmt("Unable to allocate connector ssl object");

      BIO *c_bio = nullptr, *client = nullptr, *c_bio_io = nullptr;

      // size_t bufsiz = 2048; /* small buffer for testing */

      if (!BIO_new_bio_pair(&client, 0 /*bufsiz*/, &c_bio_io, 0 /*bufsiz*/))
        throw msg_fmt("Unable to build SSL pair.");

      c_bio = BIO_new(BIO_f_ssl());
      if (!c_bio)
        throw msg_fmt("Unable to build SSL filter.");

      SSL_set_connect_state(c_ssl);
      SSL_set_bio(c_ssl, client, client);
      (void)BIO_set_ssl(c_bio, c_ssl, BIO_NOCLOSE);

      // Create stream object.
      u = std::make_unique<stream>(c_ssl, c_bio, c_bio_io);
    } catch (...) {
      // delete c_ssl;
      throw;
    }
    u->set_substream(lower);

    /* Handshake as connector */
    log_v2::tls()->error("tls before handshake");
    u->handshake();
    log_v2::tls()->error("tls after handshake");
  }

  return u;
}
