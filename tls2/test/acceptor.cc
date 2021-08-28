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
#include <chrono>

#include <nlohmann/json.hpp>

#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/misc/buffer.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"
#include "com/centreon/broker/tls2/acceptor.hh"
#include "com/centreon/broker/tls2/connector.hh"
#include "com/centreon/broker/tls2/internal.hh"
#include "com/centreon/broker/tls2/stream.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

const static std::string test_addr("127.0.0.1");
constexpr static uint16_t test_port(4444);

class Tls2Test : public ::testing::Test {
 public:
  void SetUp() override {
    pool::load(0);
    tcp::tcp_async::load();
    tls2::initialize();
  }

  void TearDown() override {
    tcp::tcp_async::unload();
    pool::unload();
  }
};

TEST_F(Tls2Test, AnonTlsStream) {
  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls2::acceptor>("", "", "", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    //tls_cbd->handshake();

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

    auto tls_c{std::make_unique<tls2::connector>("", "", "", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls2::stream* tls_centengine = static_cast<tls2::stream*>(io_tls_centengine.get());

    //tls_centengine->handshake();

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

TEST_F(Tls2Test, AnonTlsStreamContinuous) {
  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls2::acceptor>("", "", "", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    //tls_cbd->handshake();

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

    auto tls_c{std::make_unique<tls2::connector>("", "", "", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine = tls_c->open(u_centengine);
    tls2::stream* tls_centengine = static_cast<tls2::stream*>(io_tls_centengine.get());

    //tls_centengine->handshake();

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

TEST_F(Tls2Test, TlsStream) {
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

    auto tls_a{std::make_unique<tls2::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    //tls_cbd->handshake();

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

    auto tls_c{std::make_unique<tls2::connector>("/tmp/client.crt", "/tmp/client.key", "", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls2::stream* tls_centengine = static_cast<tls2::stream*>(io_tls_centengine.get());

    //tls_centengine->handshake();

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

TEST_F(Tls2Test, TlsStreamCa) {
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

    auto tls_a{std::make_unique<tls2::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "/tmp/client.crt", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    //tls_cbd->handshake();

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

    auto tls_c{std::make_unique<tls2::connector>("/tmp/client.crt", "/tmp/client.key", "/tmp/server.crt", "")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls2::stream* tls_centengine = static_cast<tls2::stream*>(io_tls_centengine.get());

    //tls_centengine->handshake();

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

TEST_F(Tls2Test, TlsStreamCaError) {
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

    auto tls_a{std::make_unique<tls2::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "/tmp/client.crt", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd;
    ASSERT_THROW(io_tls_cbd = tls_a->open(u_cbd), std::exception);
/*    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    do {
      std::shared_ptr<io::data> d;
      puts("cbd read");
      try {
        tls_cbd->read(d, 0);
      } catch (const std::exception& e) {
        std::cout << "### " << e.what() << " ###" << std::endl;
        ASSERT_EQ(strcmp(e.what(), "TLS session is terminated"), 0);
        cbd_finished = true;
        break;
      }
      std::this_thread::yield();
    } while (true);
*/
    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    auto tls_c{std::make_unique<tls2::connector>("/tmp/client.crt", "/tmp/client.key", "/tmp/server.crt", "badhostname")};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine;
    ASSERT_THROW(io_tls_centengine = tls_c->open(u_centengine), std::exception);
  });

  centengine.join();
  cbd.join();
}

TEST_F(Tls2Test, TlsStreamCaHostname) {
  /* Let's prepare certificates */
  const static std::string s_hostname{"saperlifragilistic"};
  const static std::string c_hostname{"foobar"};
  std::string server_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/server.key -out /tmp/server.crt -subj '/CN={}'", s_hostname));
  std::cout << server_cmd << std::endl;
  system(server_cmd.c_str());

  std::string client_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/client.key -out /tmp/client.crt -subj '/CN={}'", c_hostname));
  std::cout << client_cmd << std::endl;
  system(client_cmd.c_str());

  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls2::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "/tmp/client.crt", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    //tls_cbd->handshake();

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

    auto tls_c{std::make_unique<tls2::connector>("/tmp/client.crt", "/tmp/client.key", "/tmp/server.crt", s_hostname)};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls2::stream* tls_centengine = static_cast<tls2::stream*>(io_tls_centengine.get());

    //tls_centengine->handshake();

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

TEST_F(Tls2Test, TlsStreamBigData) {
  using namespace std::chrono_literals;

  /* Let's prepare certificates */
  const static std::string s_hostname{"saperlifragilistic"};
  const static std::string c_hostname{"foobar"};
  const static int max_limit = 20;
  std::string server_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/server.key -out /tmp/server.crt -subj '/CN={}'", s_hostname));
  std::cout << server_cmd << std::endl;
  system(server_cmd.c_str());

  std::string client_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/client.key -out /tmp/client.crt -subj '/CN={}'", c_hostname));
  std::cout << client_cmd << std::endl;
  system(client_cmd.c_str());

  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls2::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "/tmp/client.crt", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    char c = 'A';
    size_t length = 26u;
    std::vector<char> v(length, c);
    size_t limit = 1;
    std::vector<char> my_vector;
    do {
      std::shared_ptr<io::data> d;
      bool no_timeout = tls_cbd->read(d, 0);
      if (no_timeout) {
        io::raw* rr = static_cast<io::raw*>(d.get());
        my_vector.insert(my_vector.end(), rr->get_buffer().begin(), rr->get_buffer().end());
      }
      if (!my_vector.empty() && memcmp(my_vector.data(), v.data(), v.size()) == 0) {
        my_vector.erase(my_vector.begin(), my_vector.begin() + v.size());
        limit++;
        ASSERT_TRUE(true);
        if (limit > max_limit)
          break;
        c++;
        length *= 2u;
        v = std::vector<char>(length, c);
      }
      else if (my_vector.size() >= v.size()) {
        ASSERT_TRUE(false);
        break;
      }

      std::this_thread::yield();
    } while (true);

    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    auto tls_c{std::make_unique<tls2::connector>(
        "/tmp/client.crt", "/tmp/client.key", "/tmp/server.crt", s_hostname)};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls2::stream* tls_centengine =
        static_cast<tls2::stream*>(io_tls_centengine.get());

    char ch = 'A';
    size_t length = 26u;
    std::vector<char> v(length, ch);

    for (size_t limit = 1; limit <= max_limit;) {
      auto packet = std::make_shared<io::raw>(v);

      tls_centengine->write(packet);
      ch++;
      limit++;
      //std::this_thread::sleep_for(1ms);
      length *= 2;
      v = std::vector<char>(length, ch);
    }
    do {
      std::this_thread::yield();
    } while (!cbd_finished);
  });

  centengine.join();
  cbd.join();
}

TEST_F(Tls2Test, TlsStreamLongData) {
  using namespace std::chrono_literals;

  /* Let's prepare certificates */
  const static std::string s_hostname{"saperlifragilistic"};
  const static std::string c_hostname{"foobar"};
  const static int max_limit = 20000;
  std::string server_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/server.key -out /tmp/server.crt -subj '/CN={}'", s_hostname));
  std::cout << server_cmd << std::endl;
  system(server_cmd.c_str());

  std::string client_cmd(fmt::format("openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout /tmp/client.key -out /tmp/client.crt -subj '/CN={}'", c_hostname));
  std::cout << client_cmd << std::endl;
  system(client_cmd.c_str());

  std::atomic_bool cbd_finished{false};

  std::thread cbd([&cbd_finished] {
    auto a{std::make_unique<tcp::acceptor>(4141, -1)};

    auto tls_a{std::make_unique<tls2::acceptor>(
        "/tmp/server.crt", "/tmp/server.key", "/tmp/client.crt", "")};

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> u_cbd;
    do {
      u_cbd = a->open();
    } while (!u_cbd);

    std::unique_ptr<io::stream> io_tls_cbd = tls_a->open(u_cbd);
    tls2::stream* tls_cbd = static_cast<tls2::stream*>(io_tls_cbd.get());

    char c = 'A';
    size_t length = 26u;
    std::vector<char> v(length, c);
    size_t limit = 1;
    std::vector<char> my_vector;
    do {
      std::shared_ptr<io::data> d;
      bool no_timeout = tls_cbd->read(d, 0);
      if (no_timeout) {
        io::raw* rr = static_cast<io::raw*>(d.get());
        my_vector.insert(my_vector.end(), rr->get_buffer().begin(), rr->get_buffer().end());
      }
      if (!my_vector.empty() && memcmp(my_vector.data(), v.data(), v.size()) == 0) {
        my_vector.erase(my_vector.begin(), my_vector.begin() + v.size());
        limit++;
        ASSERT_TRUE(true);
        if (limit > max_limit)
          break;
        c++;
        if (c > 'z')
          c = 'A';
        v = std::vector<char>(length, c);
      }
      else if (my_vector.size() >= v.size()) {
        ASSERT_TRUE(false);
        break;
      }

      std::this_thread::yield();
    } while (true);

    cbd_finished = true;
  });

  std::thread centengine([&cbd_finished] {
    auto c{std::make_unique<tcp::connector>("localhost", 4141, -1)};

    auto tls_c{std::make_unique<tls2::connector>(
        "/tmp/client.crt", "/tmp/client.key", "/tmp/server.crt", s_hostname)};

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> u_centengine;
    do {
      u_centengine = c->open();
    } while (!u_centengine);

    std::unique_ptr<io::stream> io_tls_centengine{tls_c->open(u_centengine)};
    tls2::stream* tls_centengine =
        static_cast<tls2::stream*>(io_tls_centengine.get());

    char ch = 'A';
    size_t length = 26u;
    std::vector<char> v(length, ch);

    for (size_t limit = 1; limit <= max_limit;) {
      auto packet = std::make_shared<io::raw>(v);

      tls_centengine->write(packet);
      ch++;
      if (ch > 'z')
        ch = 'A';
      limit++;
      //std::this_thread::sleep_for(1ms);
      v = std::vector<char>(length, ch);
    }
    do {
      std::this_thread::yield();
    } while (!cbd_finished);
  });

  centengine.join();
  cbd.join();
}
