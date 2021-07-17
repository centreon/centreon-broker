/*
 * Copyright 2021 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/tcp/acceptor.hh"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <openssl/ssl.h>

#include <nlohmann/json.hpp>

#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/misc/buffer.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"
#include "com/centreon/broker/tls/acceptor.hh"
#include "com/centreon/broker/tls/connector.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

const static std::string test_addr("127.0.0.1");
constexpr static uint16_t test_port(4444);

class TlsTest : public ::testing::Test {
 public:
  void SetUp() override {
    pool::load(0);
    tcp::tcp_async::load();
    tls::initialize();
  }

  void TearDown() override {
    tcp::tcp_async::unload();
    pool::unload();
  }
};

TEST_F(TlsTest, Anon) {
  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::unique_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    SSL_CTX* s_ctx = SSL_CTX_new(TLS_server_method());
    ASSERT_FALSE(s_ctx == nullptr);

    /* This is needed if we want to use the aNULL cipher */
    SSL_CTX_set_security_level(s_ctx, 0);

    SSL_CTX_set_ecdh_auto(s_ctx, 1);

    ASSERT_TRUE(SSL_CTX_set_cipher_list(s_ctx, "aNULL"));

    SSL* s_ssl = SSL_new(s_ctx);
    ASSERT_TRUE(s_ssl);

    BIO* s_ssl_bio = nullptr;
    BIO *server = nullptr, *server_io = nullptr;

    size_t bufsiz = 256;
    ASSERT_TRUE(BIO_new_bio_pair(&server, bufsiz, &server_io, bufsiz));

    s_ssl_bio = BIO_new(BIO_f_ssl());
    ASSERT_TRUE(s_ssl_bio);

    SSL_set_accept_state(s_ssl);
    SSL_set_bio(s_ssl, server, server);
    (void)BIO_set_ssl(s_ssl_bio, s_ssl, BIO_NOCLOSE);

    misc::buffer rbuf;

    char sbuf[1024];

    int r;

    do {
      if (SSL_in_init(s_ssl)) {
        printf("cbd: server waiting in SSL_accept - %s\n",
               SSL_state_string_long(s_ssl));

        r = SSL_do_handshake(s_ssl);
      }
      else {
        /* We want to read something */
        r = SSL_read(s_ssl, sbuf, 20);
        printf("cbd: cbd read %d\n", r);
        if (r > 0) {
          printf("cbd: received: '%s'\n", sbuf);
          ASSERT_EQ(strncmp(sbuf, "Hello cbd", 9), 0);
          break;
        }
      }

      if (r == -1) {
        switch (SSL_get_error(s_ssl, r)) {
          case SSL_ERROR_WANT_READ:
          case SSL_ERROR_WANT_WRITE:
            printf("cbd: want read/write\n");
            {
              size_t sz;
              while ((sz = BIO_ctrl_pending(server_io)) > 0) {
                printf("cbd: want to write %lu\n", sz);
                char* dataptr;
                sz = BIO_nread(server_io, &dataptr, sz);
                auto packet = std::make_shared<io::raw>();
                packet->get_buffer().insert(packet->get_buffer().end(), dataptr, dataptr + sz);
                u_cbd->write(packet);
                printf("cbd: %lu bytes sent from cbd", sz);
              }

              while ((sz = BIO_ctrl_get_read_request(server_io)) > 0) {
                printf("cbd: want to read %lu\n", sz);
                std::shared_ptr<io::data> d;
                bool no_timeout = u_cbd->read(d, 0);
                if (no_timeout) {
                  io::raw* packet = static_cast<io::raw*>(d.get());
                  printf("cbd: %ld bytes read from cbd\n", packet->size());
                  rbuf.push(packet->get_buffer());
                  printf("cbd: rbuf total size %ld bytes\n", rbuf.size());
                }
                if (rbuf.size() > 0) {
                  int s = std::min(rbuf.size(), sz);
                  int ss = BIO_write(server_io, rbuf.pop(s).data(), s);
                  ASSERT_EQ(ss, s);
                }
                else
                  break;
              }
            }
            break;
          case SSL_ERROR_NONE:
            printf("cbd: handshake done!\n");
            break;
          default:
            printf("cbd: unknown error\n");
        }
      }
    } while (true);

    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    /* Nominal case, centengine is connector and write on the socket */
    std::unique_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    SSL_CTX* c_ctx = SSL_CTX_new(TLS_client_method());
    ASSERT_FALSE(c_ctx == nullptr);

    /* This is needed if we want to use the aNULL cipher */
    SSL_CTX_set_security_level(c_ctx, 0);

    SSL_CTX_set_ecdh_auto(c_ctx, 1);

    ASSERT_TRUE(SSL_CTX_set_cipher_list(c_ctx, "aNULL"));

    SSL* c_ssl = SSL_new(c_ctx);
    BIO* c_ssl_bio = nullptr;
    BIO *client = nullptr, *client_io = nullptr;

    size_t bufsiz = 256;
    ASSERT_TRUE(BIO_new_bio_pair(&client, bufsiz, &client_io, bufsiz));

    c_ssl_bio = BIO_new(BIO_f_ssl());
    ASSERT_TRUE(c_ssl_bio);

    SSL_set_connect_state(c_ssl);
    SSL_set_bio(c_ssl, client, client);
    (void)BIO_set_ssl(c_ssl_bio, c_ssl, BIO_NOCLOSE);

    char cbuf[20] = "Hello cbd";
    misc::buffer rbuf;

    int i = 0, r;

    /* trigger of the ssl exchanges */
    r = BIO_read(c_ssl_bio, cbuf, strlen(cbuf) + 1);
    printf("BIO_read c_ssl_bio... r = %d\n", r);

    do {
      if (SSL_in_init(c_ssl)) {
        printf("centengine: client connecting in SSL_connect - %s\n",
               SSL_state_string_long(c_ssl));

        r = SSL_do_handshake(c_ssl);
      }
      else {
        r = SSL_write(c_ssl, cbuf, strlen(cbuf) + 1);
        if (r > 0) {
          i++;
          printf("centengine: message gone '%s' r = %d\n", cbuf, r);
          //done = true;
        }
      }
      if (r == -1) {
        switch (SSL_get_error(c_ssl, r)) {
          case SSL_ERROR_WANT_READ:
          case SSL_ERROR_WANT_WRITE:
            printf("centengine: want read/write\n");
            {
              size_t sz;
              while ((sz = BIO_ctrl_pending(client_io)) > 0) {
                printf("centengine: want to write %lu\n", sz);
                char* dataptr;
                sz = BIO_nread(client_io, &dataptr, sz);
                auto packet = std::make_shared<io::raw>();
                packet->get_buffer().insert(packet->get_buffer().end(), dataptr, dataptr + sz);
                u_centengine->write(packet);
                printf("centengine: %lu bytes sent from centengine", sz);
              }

              while ((sz = BIO_ctrl_get_read_request(client_io)) > 0) {
                printf("centengine: want to read %lu\n", sz);
                std::shared_ptr<io::data> d;
                bool no_timeout = u_centengine->read(d, 0);
                if (no_timeout) {
                  io::raw* packet = static_cast<io::raw*>(d.get());
                  printf("centengine: %ld bytes read from centengine\n", packet->size());
                  rbuf.push(packet->get_buffer());
                  printf("centengine: rbuf total size %lu bytes\n", rbuf.size());
                }
                if (rbuf.size() > 0) {
                  int s = std::min(rbuf.size(), sz);
                  int ss = BIO_write(client_io, rbuf.pop(s).data(), s);
                  ASSERT_EQ(ss, s);
                }
                else
                  break;
              }
            }
            break;
          case SSL_ERROR_NONE:
            printf("centengine: handshake done!\n");
            break;
          default:
            printf("centengine: unknown error\n");
        }
      }

    } while (!cbd_finished);
  });

  centengine.join();
  cbd.join();
}

TEST_F(TlsTest, AnonTlsStream) {
  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls::acceptor>("", "", "", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls::stream* tls_cbd = static_cast<tls::stream*>(io_tls_cbd.get());

    tls_cbd->handshake();

    do {
      std::shared_ptr<io::data> d;
      puts("cbd read");
      bool no_timeout = tls_cbd->read(d, 0);
      if (no_timeout) {
        io::raw* rr = static_cast<io::raw*>(d.get());
        ASSERT_EQ(strncmp(rr->data(), "Hello cbd", 0), 0);
        break;
      }
      std::this_thread::yield();
    } while (true);

    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    auto tls_c{std::make_unique<tls::connector>("", "", "", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls::stream* tls_centengine = static_cast<tls::stream*>(io_tls_centengine.get());

    tls_centengine->handshake();

    std::vector<char> v{ 'H', 'e', 'l', 'l', 'o', ' ', 'c', 'b', 'd' };
    auto packet = std::make_shared<io::raw>(std::move(v));

    /* This is not representative of a real stream. Here we have to call write
     * several times, so that the SSL library makes its work in the back */
    do {
      puts("centengine write");
      tls_centengine->write(packet);
      std::this_thread::yield();
    } while (!cbd_finished);
  });

  centengine.join();
  cbd.join();
}

TEST_F(TlsTest, AnonTlsStreamContinuous) {
  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls::acceptor>("", "", "", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls::stream* tls_cbd = static_cast<tls::stream*>(io_tls_cbd.get());

    tls_cbd->handshake();

    int i = 0;
    char str[256];

    do {
      std::shared_ptr<io::data> d;
      bool no_timeout = tls_cbd->read(d, 0);
      if (no_timeout) {
        io::raw* rr = static_cast<io::raw*>(d.get());
        sprintf(str, "Hello cbd %d", i);
        ASSERT_EQ(strncmp(rr->data(), str, 16), 0);
        printf("=> %s\n", rr->data());
        i++;
      }
      std::this_thread::yield();
    } while (i < 500);

    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    auto tls_c{std::make_unique<tls::connector>("", "", "", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine = tls_c->open(u_centengine);
    tls::stream* tls_centengine = static_cast<tls::stream*>(io_tls_centengine.get());

    tls_centengine->handshake();

    char str[20];
    int i = 0;

    /* This is not representative of a real stream. Here we have to call write
     * several times, so that the SSL library makes its work in the back */
    do {
      sprintf(str, "Hello cbd %d", i);
      i++;
      std::vector<char> v(str, str + strlen(str) + 1);
      puts(str);
      auto packet = std::make_shared<io::raw>(std::move(v));
      tls_centengine->write(packet);
      std::this_thread::yield();
    } while (!cbd_finished);
  });

  centengine.join();
  cbd.join();
}

TEST_F(TlsTest, TlsStream) {
  /* Let's prepare certificates */
  std::string hostname = misc::exec("hostname --fqdn");
  hostname = misc::string::trim(hostname);
  std::string server_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/server.key -out /tmp/server.crt -subj '/CN={}'", hostname));
  std::cout << server_cmd << std::endl;
  system(server_cmd.c_str());

  std::string client_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/client.key -out /tmp/client.crt -subj '/CN={}'", hostname));
  std::cout << client_cmd << std::endl;
  system(client_cmd.c_str());

  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "/tmp/client.crt", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls::stream* tls_cbd = static_cast<tls::stream*>(io_tls_cbd.get());

    tls_cbd->handshake();

    do {
      std::shared_ptr<io::data> d;
      puts("cbd read");
      bool no_timeout = tls_cbd->read(d, 0);
      if (no_timeout) {
        io::raw* rr = static_cast<io::raw*>(d.get());
        ASSERT_EQ(strncmp(rr->data(), "Hello cbd", 0), 0);
        break;
      }
      std::this_thread::yield();
    } while (true);

    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    auto tls_c{std::make_unique<tls::connector>("/tmp/client.crt", "/tmp/client.key", "/tmp/server.crt", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls::stream* tls_centengine = static_cast<tls::stream*>(io_tls_centengine.get());

    tls_centengine->handshake();

    std::vector<char> v{ 'H', 'e', 'l', 'l', 'o', ' ', 'c', 'b', 'd' };
    auto packet = std::make_shared<io::raw>(std::move(v));

    /* This is not representative of a real stream. Here we have to call write
     * several times, so that the SSL library makes its work in the back */
    do {
      puts("centengine write");
      tls_centengine->write(packet);
      std::this_thread::yield();
    } while (!cbd_finished);
  });

  centengine.join();
  cbd.join();
}

