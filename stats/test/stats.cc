/*
 * Copyright 2019 - 2021 Centreon (https://www.centreon.com/)
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
#include <gtest/gtest.h>
#include <chrono>
#include <com/centreon/broker/stats/parser.hh>
#include <com/centreon/broker/stats/worker.hh>
#include <com/centreon/broker/stats/worker_pool.hh>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/factory.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/pool.hh"
#include "com/centreon/broker/stats/builder.hh"
#include "com/centreon/broker/stats/center.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

class StatsTest : public ::testing::Test {
 public:
  void SetUp() override {
    pool::load(0);
    stats::center::load();
    io::protocols::load();
    io::events::load();
    config::applier::state::load();
    multiplexing::engine::load();
    config::applier::endpoint::load();
  }

  void TearDown() override {
    config::applier::endpoint::unload();
    multiplexing::engine::unload();
    config::applier::state::unload();
    io::events::unload();
    io::protocols::unload();
    stats::center::unload();
    pool::unload();
  }
};

TEST_F(StatsTest, Builder) {
  stats::builder build;

  build.build();

  nlohmann::json const& result{nlohmann::json::parse(build.data())};

  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"], CENTREON_BROKER_VERSION);
  ASSERT_EQ(result["pid"], getpid());
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());
}

TEST_F(StatsTest, BuilderWithModules) {
  stats::builder build;
  auto& modules = config::applier::state::instance().get_modules();
  modules.apply(std::list<std::string>{}, "./storage/", nullptr);
  modules.apply(std::list<std::string>{}, "./neb/", nullptr);
  modules.apply(std::list<std::string>{}, "./lua/", nullptr);

  build.build();

  nlohmann::json result;
  ASSERT_NO_THROW(result = nlohmann::json::parse(build.data()));

  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"], CENTREON_BROKER_VERSION);
  ASSERT_EQ(result["pid"], getpid());
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());

  ASSERT_EQ(result["module./neb/10-neb.so"]["state"].get<std::string>(),
            "loaded");
  ASSERT_EQ(result["module./storage/20-storage.so"]["state"].get<std::string>(),
            "loaded");
  ASSERT_EQ(result["module./lua/70-lua.so"]["state"].get<std::string>(),
            "loaded");
}

class st : public io::stream {
 public:
  st() : io::stream("st") {}
  bool read(std::shared_ptr<io::data>& d, time_t deadline) override {
    (void)deadline;
    d.reset();
    throw exceptions::shutdown("cannot read from connector");
  }

  int32_t write(std::shared_ptr<io::data> const& d
                __attribute__((__unused__))) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return 1;
  }

  int32_t stop() override { return 0; }
};

class endp : public io::endpoint {
 public:
  endp() : io::endpoint{false} {}
  std::unique_ptr<io::stream> open() override {
    static int count = 0;
    if (++count < 2)
      return std::unique_ptr<st>(new st);
    else
      return nullptr;
  }
};

class fact : public io::factory {
 public:
  fact() {}

  bool has_endpoint(config::endpoint& cfg __attribute__((__unused__)),
                    io::extension* ext) override {
    if (ext)
      *ext = io::extension("FACT", false, false);
    return true;
  }

  io::endpoint* new_endpoint(
      config::endpoint& cfg __attribute__((__unused__)),
      bool& is_acceptor,
      __attribute__((__unused__)) std::shared_ptr<persistent_cache> cache =
          std::shared_ptr<persistent_cache>()) const override {
    endp* p{new endp()};
    is_acceptor = true;
    return p;
  }
};

TEST_F(StatsTest, BuilderWithEndpoints) {
  stats::builder build;
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  ASSERT_TRUE(file_stream);

  // Data.
  std::string data(
      "\n{"
      "  \"centreonBroker\": {\n"
      "    \"input\": {\n"
      "      \"name\": \"CentreonInput\",\n"
      "      \"type\": \"tcp\",\n"
      "      \"port\": \"5668\",\n"
      "      \"protocol\": \"bbdo\",\n"
      "      \"compression\": \"yes\"\n"
      "    },\n"
      "    \"output\": [\n"
      "      {\n"
      "        \"name\": \"CentreonDatabase\",\n"
      "        \"type\": \"sql\",\n"
      "        \"db_type\": \"mysql\",\n"
      "        \"db_host\": \"localhost\",\n"
      "        \"db_port\": \"3306\",\n"
      "        \"db_user\": \"centreon\",\n"
      "        \"db_password\": \"merethis\",\n"
      "        \"db_name\": \"centreon_storage\",\n"
      "        \"failover\": \"CentreonBrokerRRD\",\n"
      "        \"secondary_failover\": [\n"
      "          \"CentreonBrokerRRD1\",\n"
      "          \"CentreonBrokerRRD2\"\n"
      "        ],\n"
      "        \"buffering_timeout\": \"10\",\n"
      "        \"read_timeout\": \"5\",\n"
      "        \"retry_interval\": \"300\"\n"
      "      },\n"
      "      {\n"
      "        \"name\": \"CentreonBrokerRRD\",\n"
      "        \"type\": \"ipv4\",\n"
      "        \"port\": \"5670\",\n"
      "        \"host\": \"localhost\",\n"
      "        \"protocol\": \"bbdo\"\n"
      "      },\n"
      "      {\n"
      "        \"name\": \"CentreonBrokerRRD1\",\n"
      "        \"type\": \"ipv4\",\n"
      "        \"port\": \"5671\",\n"
      "        \"host\": \"localhost\",\n"
      "        \"protocol\": \"bbdo\"\n"
      "      },\n"
      "      {\n"
      "        \"name\": \"CentreonBrokerRRD2\",\n"
      "        \"type\": \"ipv4\",\n"
      "        \"port\": \"5672\",\n"
      "        \"host\": \"localhost\",\n"
      "        \"protocol\": \"bbdo\"\n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "}\n");

  // Write data.
  ASSERT_TRUE(fwrite(data.c_str(), data.size(), 1, file_stream) == 1);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  config::state s;
  ASSERT_NO_THROW(s = p.parse(config_file));

  auto test = std::make_shared<fact>();
  io::protocols::instance().reg("CentreonInput", test, 1, 7);
  io::protocols::instance().reg("CentreonDatabase", test, 1, 7);
  io::protocols::instance().reg("CentreonRetention", test, 1, 7);
  io::protocols::instance().reg("CentreonSecondaryFailover1", test, 1, 7);
  io::protocols::instance().reg("CentreonSecondaryFailover2", test, 1, 7);
  config::applier::endpoint::instance().apply(s.endpoints());

  // Remove temporary file.
  ::remove(config_file.c_str());

  build.build();

  nlohmann::json const& result{nlohmann::json::parse(build.data())};

  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"].get<std::string>(), CENTREON_BROKER_VERSION);
  ASSERT_EQ(result["pid"].get<int32_t>(), getpid());
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());
  ASSERT_TRUE(result["endpoint CentreonDatabase"]["state"].get<std::string>() ==
              "listening");
}

TEST_F(StatsTest, CopyCtor) {
  stats::builder build;

  build.build();

  stats::builder build2{build};

  std::string err;
  nlohmann::json const& result{build2.root()};

  ASSERT_TRUE(err.empty());
  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"].get<std::string>(), CENTREON_BROKER_VERSION);
  ASSERT_EQ(result["pid"].get<int32_t>(), getpid());
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());
}

TEST_F(StatsTest, Parser) {
  stats::parser parser;
  std::vector<std::string> result;
  std::vector<std::string> result2;

  parser.parse(result, "{}");
  ASSERT_TRUE(result.size() == 0);
  parser.parse(result, "{ \"json_fifo\":\"/tmp/test.txt\" }");
  ASSERT_TRUE(result.size() == 1);
  ASSERT_TRUE(result2.size() == 0);
  parser.parse(result2, "[{ \"json_fifo\":\"/tmp/test.txt\" }]");
  ASSERT_TRUE(result2.size() == 1);

  ASSERT_THROW(parser.parse(result, "ds{ahsjklhdasjhdaskjh"), msg_fmt);
}

TEST_F(StatsTest, Worker) {
  std::unique_ptr<stats::worker> work(new stats::worker);

  std::string fifo{misc::temp_path()};
  ::mkfifo(fifo.c_str(), 0777);

  work->run(fifo);
  std::ifstream f;
  f.open(fifo, std::fstream::in);

  std::string js((std::istreambuf_iterator<char>(f)),
                 std::istreambuf_iterator<char>());

  f.close();
  work.reset();

  nlohmann::json const& result{nlohmann::json::parse(js)};

  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"].get<std::string>(), CENTREON_BROKER_VERSION);
  ASSERT_EQ(result["pid"].get<int32_t>(), getpid());
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());

  std::remove(fifo.c_str());
}

TEST_F(StatsTest, WorkerPoolBadFile) {
  stats::worker_pool work;

  ASSERT_THROW(work.add_worker("/unexistingdir/file"), msg_fmt);
}

TEST_F(StatsTest, WorkerPoolExistingDir) {
  stats::worker_pool work;

  ASSERT_THROW(work.add_worker("/tmp"), msg_fmt);
}

TEST_F(StatsTest, WorkerPool) {
  std::unique_ptr<stats::worker_pool> work(new stats::worker_pool);

  std::string fifo{misc::temp_path()};
  ::mkfifo(fifo.c_str(), 0777);

  work->add_worker(fifo);
  std::ifstream f;
  f.open(fifo, std::fstream::in);

  std::string js((std::istreambuf_iterator<char>(f)),
                 std::istreambuf_iterator<char>());

  f.close();
  work.reset();

  nlohmann::json const& result{nlohmann::json::parse(js)};

  ASSERT_TRUE(result.is_object());
  ASSERT_EQ(result["version"].get<std::string>(), CENTREON_BROKER_VERSION);
  ASSERT_EQ(result["pid"].get<int32_t>(), getpid());
  ASSERT_TRUE(result["now"].is_string());
  ASSERT_TRUE(result["asio_version"].is_string());
  ASSERT_TRUE(result["mysql manager"].is_object());
  ASSERT_TRUE(result["mysql manager"]["delay since last check"].is_string());

  std::remove(fifo.c_str());
}
