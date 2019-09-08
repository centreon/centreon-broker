/*
** Copyright 2011-2013 Centreon
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

#include <gtest/gtest.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/rrd/lib.hh"

using namespace com::centreon::broker;

class Rrd : public ::testing::Test {
 public:
  void SetUp() override {
    try {
      config::applier::init();
    } catch (std::exception const& e) {
      (void)e;
    }
  }

  void TearDown() override {
    // The cache must be destroyed before the applier deinit() call.
    config::applier::deinit();
  }
};

/**
 *  Check RRD file creation.
 *
 *  @return 0 on success.
 */
TEST_F(Rrd, Create) {
  // Temporary file path.
  std::string file_path("/tmp/broker_rrd_lib_create");
  ::remove(file_path.c_str());

  // RRD library object.
  rrd::lib lib("/tmp", 16);
  lib.open(file_path, 90 * 24 * 60 * 60, time(nullptr) - 7 * 24 * 60 * 60, 60);

  // Check file exists.
  bool file_exists(!access(file_path.c_str(), F_OK));

  // Remove temporary file.
  ::remove(file_path.c_str());

  ASSERT_TRUE(file_exists);
}

TEST_F(Rrd, Remove) {
  // Temporary file path.
  std::string file_path("/tmp/broker_rrd_lib_remove");
  ::remove(file_path.c_str());

  // RRD library object.
  rrd::lib lib("/tmp", 16);
  lib.open(file_path, 90 * 24 * 60 * 60, time(nullptr) - 7 * 24 * 60 * 60, 60);

  lib.remove(file_path);

  // Check file exists.
  bool file_exists(!access(file_path.c_str(), F_OK));

  // Remove temporary file.
  ::remove(file_path.c_str());

  ASSERT_TRUE(!file_exists);
}

TEST_F(Rrd, UpdateOK) {
  // Temporary file path.
  std::string file_path("/tmp/broker_rrd_lib_update_ok");
  ::remove(file_path.c_str());

  // RRD library object.
  rrd::lib lib("/tmp", 16);
  lib.open(file_path, 90 * 24 * 60 * 60, time(nullptr) - 7 * 24 * 60 * 60, 60);

  time_t now{std::time(nullptr)};
  lib.update(now, "4.5");
  lib.update(now, "34.5");
  lib.update(now, "54.5");
  lib.update(now, "74.5");

  // Check file exists.
  bool file_exists(!access(file_path.c_str(), F_OK));

  // Remove temporary file.
  ::remove(file_path.c_str());

  ASSERT_TRUE(file_exists);
}

TEST_F(Rrd, UpdateNOK) {
  // Temporary file path.
  std::string file_path("/tmp/broker_rrd_lib_update_nok");
  ::remove(file_path.c_str());

  // RRD library object.
  rrd::lib lib("/tmp", 16);
  lib.open(file_path, 90 * 24 * 60 * 60, time(nullptr) - 7 * 24 * 60 * 60, 60);

  time_t now{std::time(nullptr)};
  lib.update(now, "");
  lib.update(now, "test");
  lib.update(now, "-ewq6we6");

  // Check file exists.
  bool file_exists(!access(file_path.c_str(), F_OK));

  // Remove temporary file.
  ::remove(file_path.c_str());

  ASSERT_TRUE(file_exists);
}