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

#include <gtest/gtest.h>
#include <cstdlib>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;

class PublisherRead : public testing::Test {
 public:
  void SetUp() override {
    config::applier::init();
  }

  void TearDown() override {
    config::applier::deinit();
  }
};

/**
 *  We should not be able to read from publisher.
 *
 */
TEST_F(PublisherRead, Read) {
  // Error flag.
  bool error(true);

  // Publisher.
  multiplexing::publisher p;

  // Read from publisher.
  try {
    std::shared_ptr<io::data> d;
    p.read(d);
    error = true;
  } catch (std::exception const& e) {
    error = false;
  } catch (...) {
    error = true;
  }

  // Return.
  ASSERT_FALSE(error);
}
