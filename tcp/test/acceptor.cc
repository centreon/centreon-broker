/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
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

#include <json11.hpp>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"

using namespace com::centreon::broker;

const static std::string test_addr("127.0.0.1");
constexpr static uint16_t test_port(4444);

static auto try_connect =
    [](tcp::connector& con) -> std::shared_ptr<io::stream> {
  std::shared_ptr<io::stream> s;
  while (!s) {
    try {
      s = con.open();
    } catch (...) {
    }
  }
  return s;
};

TEST(TcpAcceptor, BadPort) {
  if (getuid() != 0) {
    tcp::acceptor acc(2, -1);
    ASSERT_THROW(acc.open(), std::exception);
  }
}

TEST(TcpAcceptor, NoConnector) {
  tcp::acceptor acc(test_port, -1);

  ASSERT_EQ(acc.open(), std::shared_ptr<io::stream>());
}

TEST(TcpAcceptor, Nominal) {
  std::thread cbd([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> s_cbd;
    do {
      s_cbd = endp->open();
    } while (!s_cbd);

    std::shared_ptr<io::data> data_read;
    while (!data_read ||
           std::static_pointer_cast<io::raw>(data_read)->size() < 10000)
      ASSERT_NO_THROW(s_cbd->read(data_read, static_cast<time_t>(-1)));

    std::vector<char> vec(
        std::static_pointer_cast<io::raw>(data_read)->get_buffer());
    std::string result(vec.begin(), vec.end());
    char cc = 'A';
    std::string wanted;
    for (int i = 0; i < 10000; i++) {
      wanted += cc;
      if (++cc == 'z') {
        wanted += "\n";
        cc = 'A';
      }
    }
    ASSERT_EQ(wanted, result);
  });

  std::thread centengine([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    /* Nominal case, centengine is connector and write on the socket */
    std::shared_ptr<io::stream> s_centengine;
    do {
      s_centengine = endp->open();
    } while (!s_centengine);

    std::shared_ptr<io::raw> data_write{new io::raw()};
    std::string cc("A");
    for (int i = 0; i < 10000; i++) {
      data_write->append(cc);
      if (++(cc[0]) == 'z') {
        data_write->append(std::string("\n"));
        cc = "A";
      }
    }
    s_centengine->write(data_write);
    s_centengine->flush();
  });

  centengine.join();
  cbd.join();
}

TEST(TcpAcceptor, QuestionAnswer) {
  constexpr int rep = 100;

  std::thread cbd([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    /* Nominal case, cbd is acceptor and read on the socket */
    std::shared_ptr<io::stream> s_cbd;
    do {
      s_cbd = endp->open();
    } while (!s_cbd);

    std::shared_ptr<io::data> data_read;
    std::shared_ptr<io::raw> data_write;
    bool val;
    for (int i = 0; i < rep; i++) {
      val = false;
      std::string wanted(fmt::format("Question{}", i));
      while (!val || !data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() <
                 wanted.size()) {
        val = s_cbd->read(data_read, static_cast<time_t>(0));
      }

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());

      ASSERT_EQ(wanted, result);

      data_write = std::make_shared<io::raw>();
      std::string text(fmt::format("Answer{}", i));
      std::string cc("a");
      for (auto c : text) {
        cc[0] = c;
        data_write->append(cc);
      }
      s_cbd->write(data_write);
    }
    s_cbd->flush();
  });

  std::thread centengine([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    std::shared_ptr<io::stream> s_centengine;
    do {
      s_centengine = endp->open();
    } while (!s_centengine);

    std::shared_ptr<io::data> data_read;
    std::shared_ptr<io::raw> data_write;
    bool val;
    for (int i = 0; i < rep; i++) {
      data_write = std::make_shared<io::raw>();
      std::string text(fmt::format("Question{}", i));
      std::string cc("a");
      for (auto c : text) {
        cc[0] = c;
        data_write->append(cc);
      }
      s_centengine->write(data_write);

      val = false;
      std::string wanted(fmt::format("Answer{}", i));
      while (!val || !data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() <
                 wanted.size())
        val = s_centengine->read(data_read, static_cast<time_t>(0));

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());

      ASSERT_EQ(wanted, result);
    }
  });

  centengine.join();
  cbd.join();
}

TEST(TcpAcceptor, MultiNominal) {
  constexpr size_t nb_poller(10);
  std::mutex cbd_m;
  std::unique_lock<std::mutex> lock(cbd_m);
  std::condition_variable cbd_cv;
  bool cbd_finished = false;

  std::thread cbd([&nb_poller, &cbd_finished, &cbd_m, &cbd_cv] {
    char cc = 'A';
    std::string wanted;
    for (int i = 0; i < 10000; i++) {
      wanted += cc;
      if (++cc == 'z') {
        wanted += "\n";
        cc = 'A';
      }
    }

    std::vector<std::string> data(nb_poller);
    {
      std::vector<std::shared_ptr<io::stream>> s_cbd(nb_poller);
      std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
      std::unique_ptr<io::endpoint> endp(a.release());

      /* Nominal case, cbd is acceptor and read on the socket */
      bool cont = true;

      while (cont) {
        std::cout << "Open stream\n";
        auto s(endp->open());
        if (s) {
          std::cout << "Open stream done\n";
          for (auto it = s_cbd.begin(); it != s_cbd.end(); ++it) {
            if (!*it) {
              *it = s;
              break;
            }
          }
        }
        std::cout << "before reading\n";
        cont = false;
        for (size_t i = 0; i < nb_poller; i++) {
          std::cout << "READING i = " << i << "\n";
          auto s = s_cbd[i];
          if (s) {
            std::shared_ptr<io::data> d;
            /* The read function does not wait... */
            s->read(d, static_cast<time_t>(0));
            std::vector<char> vec(
                std::static_pointer_cast<io::raw>(d)->get_buffer());
            data[i].insert(data[i].end(), vec.begin(), vec.end());
            if (data[i].size() < wanted.size()) {
              std::cout << "not finished to fill stream...\n";
              cont = true;
            }
          } else {
            std::cout << "not finished...\n";
            cont = true;
            break;
          }
        }
        std::cout << "after reading, cont = " << cont << "\n";
      }

      std::cout << "Checking...\n";
      ASSERT_EQ(s_cbd.size(), nb_poller);
    }

    for (auto d : data)
      ASSERT_EQ(wanted, d);

    std::lock_guard<std::mutex> lock(cbd_m);
    cbd_finished = true;
    cbd_cv.notify_all();
  });

  std::vector<std::thread> pollers;

  for (size_t i = 0; i < nb_poller; i++) {
    pollers.emplace_back([&cbd_finished, &cbd_m, &cbd_cv] {
      std::unique_ptr<tcp::connector> c(
          new tcp::connector("localhost", 4141, -1));
      std::unique_ptr<io::endpoint> endp(c.release());

      /* Nominal case, centengine is connector and write on the socket */
      std::shared_ptr<io::stream> s_centengine;
      do {
        s_centengine = endp->open();
      } while (!s_centengine);

      std::shared_ptr<io::raw> data_write{new io::raw()};
      std::string cc("A");
      for (int i = 0; i < 10000; i++) {
        data_write->append(cc);
        if (++(cc[0]) == 'z') {
          data_write->append(std::string("\n"));
          cc = "A";
        }
      }
      s_centengine->write(data_write);
      s_centengine->flush();
      std::unique_lock<std::mutex> lock(cbd_m);
      cbd_cv.wait(lock, [&cbd_finished] { return cbd_finished; });
    });
  }

  cbd_cv.wait(lock, [&cbd_finished] { return cbd_finished; });
  lock.unlock();

  std::cout << "Stopping pollers\n";
  for (auto& p : pollers) {
    p.join();
    std::cout << "Done.\n";
  }
  std::cout << "All pollers stopped.\n";

  cbd.join();
}

TEST(TcpAcceptor, NominalReversed) {
  std::thread centengine([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    std::shared_ptr<io::stream> s_centengine;
    do {
      try {
        s_centengine = endp->open();
      } catch (const std::exception& e) {
        std::cout << '.';
      }
    } while (!s_centengine);
    std::cout << '\n';

    std::shared_ptr<io::raw> data_write{new io::raw()};
    std::string cc("A");
    for (int i = 0; i < 10000; i++) {
      data_write->append(cc);
      if (++(cc[0]) == 'z') {
        data_write->append(std::string("\n"));
        cc = "A";
      }
    }
    s_centengine->write(data_write);
    s_centengine->flush();
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::thread cbd([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    std::shared_ptr<io::stream> s_cbd;
    do {
      s_cbd = endp->open();
    } while (!s_cbd);

    std::shared_ptr<io::data> data_read;
    while (!data_read ||
           std::static_pointer_cast<io::raw>(data_read)->size() < 10000)
      s_cbd->read(data_read, static_cast<time_t>(-1));

    std::vector<char> vec(
        std::static_pointer_cast<io::raw>(data_read)->get_buffer());
    std::string result(vec.begin(), vec.end());
    char cc = 'A';
    std::string wanted;
    for (int i = 0; i < 10000; i++) {
      wanted += cc;
      if (++cc == 'z') {
        wanted += "\n";
        cc = 'A';
      }
    }
    ASSERT_EQ(wanted, result);
  });

  cbd.join();
  centengine.join();
}

TEST(TcpAcceptor, OnePeer) {
  std::thread centengine([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    std::shared_ptr<io::stream> s_centengine;
    do {
      s_centengine = endp->open();
    } while (!s_centengine);

    std::shared_ptr<io::raw> data_write{new io::raw()};
    std::string cc("A");
    for (int i = 0; i < 10000; i++) {
      data_write->append(cc);
      if (++(cc[0]) == 'z') {
        data_write->append(std::string("\n"));
        cc = "A";
      }
    }
    s_centengine->write(data_write);
    s_centengine->flush();
  });

  std::thread cbd([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    std::shared_ptr<io::stream> s_cbd;
    do {
      s_cbd = endp->open();
    } while (!s_cbd);

    std::shared_ptr<io::data> data_read;
    while (!data_read ||
           std::static_pointer_cast<io::raw>(data_read)->size() < 10000)
      s_cbd->read(data_read, static_cast<time_t>(-1));

    std::vector<char> vec(
        std::static_pointer_cast<io::raw>(data_read)->get_buffer());
    std::string result(vec.begin(), vec.end());
    char cc = 'A';
    std::string wanted;
    for (int i = 0; i < 10000; i++) {
      wanted += cc;
      if (++cc == 'z') {
        wanted += "\n";
        cc = 'A';
      }
    }
    ASSERT_EQ(wanted, result);
  });

  cbd.join();
  centengine.join();
}

TEST(TcpAcceptor, OnePeerReversed) {
  std::thread cbd([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    std::shared_ptr<io::stream> s_cbd;
    do {
      try {
        s_cbd = endp->open();
      } catch (const std::exception& e) {
        std::cout << '.';
      }
    } while (!s_cbd);
    std::cout << '\n';

    std::shared_ptr<io::data> data_read;
    while (!data_read ||
           std::static_pointer_cast<io::raw>(data_read)->size() < 10000)
      s_cbd->read(data_read, static_cast<time_t>(-1));

    std::vector<char> vec(
        std::static_pointer_cast<io::raw>(data_read)->get_buffer());
    std::string result(vec.begin(), vec.end());
    char cc = 'A';
    std::string wanted;
    for (int i = 0; i < 10000; i++) {
      wanted += cc;
      if (++cc == 'z') {
        wanted += "\n";
        cc = 'A';
      }
    }
    ASSERT_EQ(wanted, result);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::thread centengine([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    std::shared_ptr<io::stream> s_centengine;
    do {
      s_centengine = endp->open();
    } while (!s_centengine);

    std::shared_ptr<io::raw> data_write{new io::raw()};
    std::string cc("A");
    for (int i = 0; i < 10000; i++) {
      data_write->append(cc);
      if (++(cc[0]) == 'z') {
        data_write->append(std::string("\n"));
        cc = "A";
      }
    }
    s_centengine->write(data_write);
    s_centengine->flush();
  });

  centengine.join();
  cbd.join();
}

TEST(TcpAcceptor, MultiOnePeer) {
  const int nb_steps = 5;

  std::thread centengine([] {
    std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
    std::unique_ptr<io::endpoint> endp(a.release());

    std::shared_ptr<io::stream> s_centengine;

    int i = 0;
    while (i < nb_steps) {
      if (s_centengine) {
        std::shared_ptr<io::data> data_read;
        while (!data_read ||
               std::static_pointer_cast<io::raw>(data_read)->size() == 0) {
          try {
            s_centengine->read(data_read, static_cast<time_t>(0));
          } catch (const std::exception& e) {
            s_centengine.reset();
          }
        }

        std::vector<char> vec(
            std::static_pointer_cast<io::raw>(data_read)->get_buffer());
        std::string result(vec.begin(), vec.end());
        ASSERT_EQ(vec.size(), 7u);
        ASSERT_EQ(result, "Hello1!");
        std::shared_ptr<io::raw> data_write = std::make_shared<io::raw>();
        data_write->append(std::string("Hello2!"));
        s_centengine->write(data_write);
        s_centengine->flush();
        s_centengine.reset();
        i++;
      } else
        s_centengine = endp->open();
    }
  });

  /* We start nb_steps instances of cbd one after the other. Each time, it
   * simulates a negotiation with the centengine instance */
  for (int i = 0; i < nb_steps; i++) {
    std::thread cbd([] {
      std::unique_ptr<tcp::connector> c(
          new tcp::connector("localhost", 4141, -1));
      std::unique_ptr<io::endpoint> endp(c.release());

      std::shared_ptr<io::stream> s_cbd;
      do {
        s_cbd = endp->open();
      } while (!s_cbd);

      std::shared_ptr<io::raw> data_write = std::make_shared<io::raw>();
      data_write->append(std::string("Hello1!"));
      ASSERT_NO_THROW(s_cbd->write(data_write));

      std::shared_ptr<io::data> data_read;
      while (!data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() == 0) {
        ASSERT_NO_THROW(s_cbd->read(data_read, static_cast<time_t>(-1)));
      }

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());
      ASSERT_EQ(result, "Hello2!");
    });

    cbd.join();
  }
  centengine.join();
}

TEST(TcpAcceptor, NominalRepeated) {
  const int nb_steps = 5;

  std::thread centengine([] {
    std::unique_ptr<tcp::connector> c(
        new tcp::connector("localhost", 4141, -1));
    std::unique_ptr<io::endpoint> endp(c.release());

    std::shared_ptr<io::stream> s_centengine;

    int i = 0;
    do {
      s_centengine = endp->open();
    } while (!s_centengine);

    while (i < nb_steps) {
      std::cout << "engine 0 " << i << "\n";
      std::shared_ptr<io::data> data_read;
      while (!data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() == 0) {
        std::cout << "engine 1 " << i << "\n";
        try {
          s_centengine->read(data_read, static_cast<time_t>(-1));
        } catch (const std::exception& e) {
          do {
            s_centengine = endp->open();
          } while (!s_centengine);
        }
      }
      std::cout << "engine 2 " << i << "\n";

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());
      ASSERT_EQ(result, "Hello1!");
      std::cout << "engine 3 " << i << "\n";
      std::shared_ptr<io::raw> data_write = std::make_shared<io::raw>();
      data_write->append(std::string("Hello2!"));
      s_centengine->write(data_write);
      std::cout << "engine 4 " << i << "\n";
      i++;
    }
    s_centengine->flush();
  });

  /* We start nb_steps instances of cbd one after the other. Each time, it
   * simulates a negotiation with the centengine instance */
  for (int i = 0; i < nb_steps; i++) {
    std::thread cbd([i] {
      std::cout << "cbd  " << i << "\n";
      std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141, -1));
      std::cout << "cbd1 " << i << "\n";
      std::cout << "cbd2 " << i << "\n";
      std::cout << "cbd3 " << i << "\n";
      std::unique_ptr<io::endpoint> endp(a.release());
      std::cout << "cbd4 " << i << "\n";

      std::shared_ptr<io::stream> s_cbd;
      do {
        s_cbd = endp->open();
      } while (!s_cbd);

      std::cout << "cbd5 " << i << "\n";
      std::shared_ptr<io::raw> data_write = std::make_shared<io::raw>();
      data_write->append(std::string("Hello1!"));
      ASSERT_NO_THROW(s_cbd->write(data_write));

      std::shared_ptr<io::data> data_read;
      while (!data_read ||
             std::static_pointer_cast<io::raw>(data_read)->size() == 0) {
        try {
          s_cbd->read(data_read, static_cast<time_t>(-1));
        } catch (const std::exception& e) {
          std::cout << "EXCEPTION DURING READING: " << e.what() << "\n";
          break;
        }
      }

      std::vector<char> vec(
          std::static_pointer_cast<io::raw>(data_read)->get_buffer());
      std::string result(vec.begin(), vec.end());
      ASSERT_EQ(result, "Hello2!");
    });

    cbd.join();
  }
  centengine.join();
}

TEST(TcpAcceptor, Wait2Connect) {
  tcp::acceptor acc(4141, -1);
  int i = 0;
  std::shared_ptr<io::stream> st;

  std::thread t{[&] {
    std::this_thread::sleep_for(std::chrono::seconds{2});
    tcp::connector con(test_addr, 4141, -1);
    std::shared_ptr<io::stream> str{try_connect(con)};
  }};

  while (!st) {
    std::cout << "TRY " << i << "\n";
    i++;
    try {
      st = acc.open();
    } catch (std::exception const& e) {
      std::cout << std::this_thread::get_id() << "EXCEPTION: " << e.what()
                << "\n";
    }
  }
  t.join();
  ASSERT_GT(i, 0);
}

TEST(TcpAcceptor, Simple) {
  tcp::acceptor acc(test_port, -1);
  std::condition_variable cv;
  std::mutex m;
  bool finish = false;

  std::thread t([&] {
    tcp::connector con(test_addr, test_port, -1);
    std::shared_ptr<io::stream> str{try_connect(con)};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    data->append(std::string("TEST\n"));
    str->write(data);
    str->flush();
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&finish] { return finish; });
  });

  {
    std::lock_guard<std::mutex> lock(m);
    std::shared_ptr<io::stream> io;
    for (;;) {
      io = acc.open();
      if (io)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;

    while (!io->read(data_read, -1)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      std::cout << std::this_thread::get_id() << "loop... ";
    }

    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "TEST\n");
    data->append(std::string("TEST\n"));
    io->write(data);
  }
  finish = true;
  cv.notify_one();
  t.join();
}

TEST(TcpAcceptor, Multiple) {
  tcp::acceptor acc(test_port, -1);

  {
    std::thread t{[] {
      tcp::connector con(test_addr, test_port, -1);
      std::shared_ptr<io::stream> str{try_connect(con)};
      std::shared_ptr<io::raw> data{new io::raw()};
      std::shared_ptr<io::data> data_read;
      data->append(std::string("TEST\n"));
      str->write(data);
      str->read(data_read, -1);
    }};
    std::shared_ptr<io::stream> io;
    for (;;) {
      io = acc.open();
      if (io)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    io->read(data_read);

    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "TEST\n");

    data->append(std::string("TEST\n"));
    io->write(data);

    t.join();
  }
  {
    std::thread t{[] {
      tcp::connector con(test_addr, test_port, -1);
      std::shared_ptr<io::stream> str{try_connect(con)};
      std::shared_ptr<io::raw> data{new io::raw()};
      std::shared_ptr<io::data> data_read;
      data->append(std::string("TEST\n"));
      str->write(data);
      str->read(data_read, -1);
    }};
    std::shared_ptr<io::stream> io;
    for (;;) {
      io = acc.open();
      if (io)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    io->read(data_read);

    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
    std::string str{vec.begin(), vec.end()};
    ASSERT_TRUE(str == "TEST\n");

    data->append(std::string("TEST\n"));
    io->write(data);

    t.join();
  }
}

TEST(TcpAcceptor, BigSend) {
  tcp::acceptor acc(test_port, -1);

  std::thread t{[] {
    tcp::connector con(test_addr, test_port, -1);
    std::shared_ptr<io::stream> str{try_connect(con)};
    std::shared_ptr<io::raw> data{new io::raw()};
    std::shared_ptr<io::data> data_read;
    for (int i = 0; i < 1024; i++) {
      data->append(std::string("0123456789"));
    }
    data->append(std::string("01234"));
    str->write(data);
    str->read(data_read, -1);
  }};
  std::shared_ptr<io::stream> io;
  for (;;) {
    io = acc.open();
    if (io)
      break;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data_read;
  io->read(data_read, time(nullptr) + 5);

  std::vector<char> vec{
      std::static_pointer_cast<io::raw>(data_read)->get_buffer()};
  std::string str{vec.begin(), vec.end()};

  data->append(std::string("TEST\n"));
  io->write(data);

  ASSERT_TRUE(str.length() == 10245);

  t.join();
}

TEST(TcpAcceptor, CloseRead) {
  tcp::acceptor acc(test_port, -1);

  std::thread t{[&] {
    {
      tcp::connector con(test_addr, test_port, -1);
      std::shared_ptr<io::stream> str{try_connect(con)};
      std::shared_ptr<io::raw> data{new io::raw()};
      std::shared_ptr<io::data> data_read;
      data->append(std::string("0"));
      str->write(data);
      str->flush();
    }
  }};
  std::shared_ptr<io::stream> io;
  for (;;) {
    io = acc.open();
    if (io)
      break;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  std::shared_ptr<io::raw> data{new io::raw()};
  std::shared_ptr<io::data> data_read;

  t.join();
  for (;;) {
    try {
      io->read(data_read, -1);
    } catch (exceptions::msg const& ex) {
      break;
    }
  }
}

TEST(TcpAcceptor, ChildsAndStats) {
  tcp::acceptor acc(test_port, -1);

  acc.add_child("child1");
  acc.add_child("child2");
  acc.add_child("child3");
  acc.remove_child("child2");

  json11::Json::object obj;
  acc.stats(obj);
  json11::Json js{obj};
  ASSERT_EQ(js.dump(), "{\"peers\": \"2: child1, child3\"}");
}

TEST(TcpAcceptor, QuestionAnswerMultiple) {
  constexpr int nb_connections = 5;
  constexpr int rep = 100;
  std::vector<std::thread> cbd, centengine;

  for (int i = 0; i < nb_connections; i++) {
    cbd.emplace_back([i] {
      std::unique_ptr<tcp::acceptor> a(new tcp::acceptor(4141 + i, -1));
      std::unique_ptr<io::endpoint> endp(a.release());

      /* Nominal case, cbd is acceptor and read on the socket */
      std::shared_ptr<io::stream> s_cbd;
      do {
        s_cbd = endp->open();
      } while (!s_cbd);

      std::shared_ptr<io::data> data_read;
      std::shared_ptr<io::raw> data_write;
      bool val;
      for (int i = 0; i < rep; i++) {
        val = false;
        std::string wanted(fmt::format("Question{}", i));
        while (!val || !data_read ||
               std::static_pointer_cast<io::raw>(data_read)->size() <
                   wanted.size()) {
          val = s_cbd->read(data_read, static_cast<time_t>(0));
        }

        std::vector<char> vec(
            std::static_pointer_cast<io::raw>(data_read)->get_buffer());
        std::string result(vec.begin(), vec.end());

        ASSERT_EQ(wanted, result);

        data_write = std::make_shared<io::raw>();
        std::string text(fmt::format("Answer{}", i));
        std::string cc("a");
        for (auto c : text) {
          cc[0] = c;
          data_write->append(cc);
        }
        s_cbd->write(data_write);
      }
      s_cbd->flush();
    });

    centengine.emplace_back([i] {
      std::unique_ptr<tcp::connector> c(
          new tcp::connector("localhost", 4141 + i, -1));
      std::unique_ptr<io::endpoint> endp(c.release());

      std::shared_ptr<io::stream> s_centengine;
      do {
        s_centengine = endp->open();
      } while (!s_centengine);

      std::shared_ptr<io::data> data_read;
      std::shared_ptr<io::raw> data_write;
      bool val;
      for (int i = 0; i < rep; i++) {
        data_write = std::make_shared<io::raw>();
        std::string text(fmt::format("Question{}", i));
        std::string cc("a");
        for (auto c : text) {
          cc[0] = c;
          data_write->append(cc);
        }
        s_centengine->write(data_write);

        val = false;
        std::string wanted(fmt::format("Answer{}", i));
        while (!val || !data_read ||
               std::static_pointer_cast<io::raw>(data_read)->size() <
                   wanted.size())
          val = s_centengine->read(data_read, static_cast<time_t>(0));

        std::vector<char> vec(
            std::static_pointer_cast<io::raw>(data_read)->get_buffer());
        std::string result(vec.begin(), vec.end());

        ASSERT_EQ(wanted, result);
      }
    });
  }

  for (int i = 0; i < nb_connections; i++) {
    centengine[i].join();
    cbd[i].join();
  }
}

TEST(TcpAcceptor, MultipleBigSend) {
  tcp::acceptor acc(test_port, -1);
  const int32_t nb_packet = 10;
  const int32_t len = 10024;

  std::thread t{[nb_packet] {
    tcp::connector con(test_addr, test_port, -1);
    std::shared_ptr<io::stream> str{try_connect(con)};
    std::shared_ptr<io::data> data_read;
    for (int k = 0; k < nb_packet; k++) {
      std::shared_ptr<io::raw> data = std::make_shared<io::raw>();
      for (int i = 0; i < len; i++) {
        std::string a(10, '0' + k);
        data->append(a);
      }
      str->write(data);
    }
    str->read(data_read, -1);
  }};
  std::shared_ptr<io::stream> io;
  for (;;) {
    io = acc.open();
    if (io)
      break;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  std::shared_ptr<io::data> data;
  std::shared_ptr<io::raw> data_read = std::make_shared<io::raw>();

  do {
    io->read(data, time(nullptr) + 5);
    std::vector<char> vec{
        std::static_pointer_cast<io::raw>(data)->get_buffer()};
    data_read->append(vec);
  } while (data_read->size() < nb_packet * len * 10);
  std::string str{data_read->get_buffer().begin(),
                  data_read->get_buffer().end()};

  int k = -1;
  for (int i = 0; i < nb_packet * len * 10; i++) {
    if (i % (len * 10) == 0)
      k++;
    ASSERT_EQ(str[i], '0' + k);
  }
  ASSERT_TRUE(str.length() == nb_packet * len * 10);

  std::static_pointer_cast<io::raw>(data)->get_buffer().clear();
  std::static_pointer_cast<io::raw>(data)->append(std::string("TEST\n"));
  io->write(data);

  t.join();
}
