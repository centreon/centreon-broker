/*
 * Copyright 2019 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/brokerrpc.hh"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <json11.hpp>

#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/version.hh"

using namespace com::centreon;
using namespace com::centreon::broker;

class BrokerRpc : public ::testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}

  std::list<std::string> execute(const std::string& command) {
    std::list<std::string> retval;
    char path[1024];
    std::ostringstream oss;
    oss << "test/rpc_client " << command;

    FILE* fp = popen(oss.str().c_str(), "r");
    while (fgets(path, sizeof(path), fp) != nullptr) {
      size_t count = strlen(path);
      if (count > 0)
        --count;
      retval.push_back(std::string(path, count));
    }
    pclose(fp);
    return retval;
  }
};

TEST_F(BrokerRpc, StartStop) {
  brokerrpc brpc("0.0.0.0", 40000, "test");
  ASSERT_NO_THROW(brpc.shutdown());
}

TEST_F(BrokerRpc, GetVersion) {
  std::ostringstream oss;
  oss << "GetVersion: major: " << version::major;
  brokerrpc brpc("0.0.0.0", 40000, "test");
  auto output = execute("GetVersion");
#if CENTREON_BROKER_PATCH == 0
  ASSERT_EQ(output.size(), 2);
  ASSERT_EQ(output.front(), oss.str());
  oss.str("");
  oss << "minor: " << version::minor;
  ASSERT_EQ(output.back(), oss.str());
#else
  ASSERT_EQ(output.size(), 3);
  ASSERT_EQ(output.front(), oss.str());
  oss.str("");
  oss << "patch: " << version::patch;
  ASSERT_EQ(output.back(), oss.str());
#endif
  brpc.shutdown();
}

TEST_F(BrokerRpc, ConfReloadBad) {
  brokerrpc brpc("0.0.0.0", 40000, "test");
  auto output = execute("DebugConfReload /root/testfail.json");
  ASSERT_EQ(output.size(), 1);
  ASSERT_EQ(output.back(),
            "DebugConfReload failed for file /root/testfail.json : file '/root/testfail.json' does not exist");
  brpc.shutdown();
}

TEST_F(BrokerRpc, ConfReloadOK) {
  json11::Json my_json = json11::Json::object{
      {"console", true}, {"loggers", json11::Json::array{}}};
  std::string output_str;
  my_json.dump(output_str);

  std::remove("/tmp/testok.json");
  std::ofstream conf("/tmp/testok.json");
  conf << output_str;
  conf.close();

  testing::internal::CaptureStdout();

  brokerrpc brpc("0.0.0.0", 40000, "broker");
  auto output = execute("DebugConfReload /tmp/testok.json");
  ASSERT_EQ(output.size(), 1);
  ASSERT_EQ(output.back(), "DebugConfReload OK");
  log_v2::core()->info("test");
  brpc.shutdown();

  std::string log_output = testing::internal::GetCapturedStdout();

  ASSERT_NE(log_output.find("] broker :"), std::string::npos);
  ASSERT_NE(log_output.find("] test"), std::string::npos);

  std::remove("/tmp/testok.json");
}
