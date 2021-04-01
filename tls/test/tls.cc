/*
 * Copyright 2011 - 2020 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/opensslconf.h>
#include <openssl/ssl.h>

#include <fstream>
#include <list>
#include <memory>
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/misc/variant.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/persistent_file.hh"
#include "com/centreon/broker/tls/params.hh"
#include "com/centreon/broker/tls/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::tls;

class ssl_memory : public io::stream {
 public:
  ssl_memory() : io::stream("ssl_memory"), _memory() {}
  ~ssl_memory() override {}

  bool read(std::shared_ptr<io::data>& d,
            time_t deadline = (time_t)-1) override {
    (void)deadline;
    if (_memory.empty())
      return false;
    std::shared_ptr<io::raw> raw(new io::raw);
    raw->get_buffer() = std::move(_memory);
    _memory.clear();
    d = raw;
    return true;
  }

  int write(std::shared_ptr<io::data> const& d) override {
    if (!validate(d, get_name()))
      return 1;
    std::cout << "on write memory 0" << std::endl;
    _memory = std::static_pointer_cast<io::raw>(d)->get_buffer();
    std::cout << "on write memory 2" << std::endl;
    return 1;
  }

  std::vector<char> const& get_memory() const { return _memory; }
  std::vector<char>& get_mutable_memory() { return _memory; }

 private:
  std::vector<char> _memory;
};

class TlsTest : public ::testing::Test {
 public:
  void SetUp() override {
    io::data::broker_id = 0;
    try {
      config::applier::init();
    } catch (std::exception const& e) {
      (void)e;
    }
    std::shared_ptr<persistent_cache> pcache(
        std::make_shared<persistent_cache>("/tmp/broker_test_cache"));
  }

  void TearDown() override {
    // The cache must be destroyed before the applier deinit() call.
    config::applier::deinit();
    ::remove("/tmp/broker_test_cache");
    ::remove(log_v2::instance().log_name().c_str());
  }
};

TEST_F(TlsTest, WriteReadService) {
  std::shared_ptr<io::stream> stream;
  std::shared_ptr<ssl_memory> memory_stream(new ssl_memory());

  SSL* ssl;
  std::cout << "debut 4" << std::endl;
  SSL_CTX* ctx;
  // OpenSSL_add_all_algorithms(); /* Load cryptos, et.al. */
  SSL_load_error_strings(); /* Bring in and register error messages */
  ctx = SSL_CTX_new(TLS_client_method()); /* Create new context */
  std::cout << "debut 5" << std::endl;
  if (ctx == NULL) {
    // ERR_print_errors_fp(stderr);
    std::cout << "TLS: on params init ctx rateeeeeeee" << std::endl;
    abort();
  }

  SSL_CTX_set_max_cert_list(ctx, TLS1_2_VERSION);
  SSL_CTX_set_cipher_list(ctx, "AES128-SHA");
  SSL_CTX_set_ciphersuites(ctx, "TLS_AES_128_GCM_SHA256");
  ERR_clear_error();
  params p(params::SERVER);
  p.set_cert("/opt/centreon-broker/test/python/server.crt",
             "/opt/centreon-broker/test/python/server.key");
  p.set_trusted_ca("/opt/centreon-broker/test/python/client.crt");
  p.load(ctx);
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
  SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

  ssl = SSL_new(ctx); /* create new SSL connection state */
  BIO* rbio = BIO_new(BIO_s_mem());
  BIO* wbio = BIO_new(BIO_s_mem());
  SSL_set_accept_state(ssl);
  // SSL_set_max_version(ssl, TLS1_2_VERSION);
  // SSL_set_tls_channel_id_enabled(ssl, 1);

  // Enable ciphers that are off by default.
  // SSL_set_strict_cipher_list(ssl, "ALL:NULL-SHA");

  DH* dh = DH_get_1024_160();
  SSL_set_tmp_dh(ssl, dh);
  DH_free(dh);
  std::cout << "handshake  " << SSL_do_handshake(ssl) << std::endl;
  SSL_set_bio(ssl, rbio, wbio);
  std::cout << "debut 7" << std::endl;
  std::shared_ptr<io::data> d;
  d.reset();
  std::shared_ptr<io::raw> raw(new io::raw);
  std::vector<char> buffer({'c', 'o', 'u'});
  raw->get_buffer() = buffer;
  d = raw;

  tls::stream* stm = new tls::stream(ssl, rbio, wbio);
  stm->set_substream(memory_stream);
  stm->write(d);

  std::cout << "debut 8" << std::endl;
  std::shared_ptr<io::data> e;
  stm->read(e, time(nullptr) + 1000);
  std::cout << "debut 9" << std::endl;
  ASSERT_EQ(std::static_pointer_cast<io::raw>(d)->get_buffer(),
            std::static_pointer_cast<io::raw>(e)->get_buffer());
  std::cout << "debut 11" << std::endl;
}
