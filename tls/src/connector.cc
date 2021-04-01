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

#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/opensslconf.h>
#include <openssl/ssl.h>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/params.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;
using namespace com::centreon::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor
 *
 *  @param[in] cert Certificate.
 *  @param[in] key  Key file.
 *  @param[in] ca   Trusted CA's certificate.
 */
connector::connector(std::string const& cert,
                     std::string const& key,
                     std::string const& ca)
    : io::endpoint(false), _ca(ca), _cert(cert), _key(key) {}

/**
 *  Connect to the remote TLS peer.
 *
 *  @return New connected stream.
 */
std::shared_ptr<io::stream> connector::open() {
  log_v2::tls()->info("TLS: on connector open()");
  // First connect the lower layer.
  std::shared_ptr<io::stream> lower(_from->open());
  std::shared_ptr<io::stream> new_stream;
  if (lower)
    new_stream = open(lower);
  return new_stream;
}

/**
 *  Overload of open, using base stream.
 *
 *  @param[in] lower Open stream.
 *
 *  @return Encrypted stream.
 */
std::shared_ptr<io::stream> connector::open(std::shared_ptr<io::stream> lower) {
  std::shared_ptr<io::stream> s;
  // if (lower) {
  int ret;
  OPENSSL_init_ssl(OPENSSL_INIT_LOAD_CONFIG, NULL);
  // SSL_library_init();
  int server;
  SSL* ssl;
  log_v2::tls()->info("TLS: on connector open(param)");

  params p(params::CLIENT);
  const SSL_METHOD* method;
  SSL_CTX* ctx;
  OpenSSL_add_all_algorithms(); /* Load cryptos, et.al. */
  SSL_load_error_strings();     /* Bring in and register error messages */
  ctx = SSL_CTX_new(TLS_client_method()); /* Create new context */

  if (ctx == NULL) {
    // ERR_print_errors_fp(stderr);
    log_v2::tls()->info("TLS: on params init ctx rateeeeeeee");
    abort();
  }
  SSL_CTX_set_max_cert_list(ctx, TLS1_2_VERSION);
  SSL_CTX_set_cipher_list(ctx, "AES128-SHA");
  ERR_clear_error();

  p.set_cert(_cert, _key);
  p.set_trusted_ca(_ca);
  p.load(ctx);
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  log_v2::tls()->info("TLS: on acceptor open avant openlistener ");
  ssl = SSL_new(ctx); /* create new SSL connection state */
  log_v2::tls()->info("TLS: on acceptor open apres new ssl ");

  BIO* rbio;
  BIO* wbio;

  SSL_set_bio(ssl, rbio, wbio);

  log_v2::tls()->debug("TLS: initializing session 1");
  s = std::shared_ptr<io::stream>(new stream(ssl, rbio, wbio));

  s->set_substream(lower);

  return s;
  //}
}
