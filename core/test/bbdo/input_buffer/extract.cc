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
#include "com/centreon/broker/bbdo/input_buffer.hh"

using namespace com::centreon::broker;

class BbdoInputBufferExtract : public ::testing::Test {
 public:
  void SetUp() override {
    for (int i = 1; i < 120; ++i) {
      std::vector<char> buffer;
      buffer.reserve(120);
      for (int j = 0; j < i; ++j)
        buffer.push_back(j);
      _raw.append(buffer.data(), i);
      _buffer.append(std::move(buffer));
    }
  }

 protected:
  bbdo::input_buffer _buffer;
  std::string _raw;
};

// Given a bbdo::input_buffer object filled with data
// When extract() is called with an offset of 0 and a size of 1
// Then the first byte is extracted
TEST_F(BbdoInputBufferExtract, Offset0Size1) {
  std::string output;
  _buffer.extract(output, 0, 1);
  ASSERT_EQ(output.size(), 1u);
  ASSERT_EQ(output[0], 0);
}

// Given a bbdo::input_buffer object filled with data
// When extract is called with an offset of 10 and a size of 46
// Then the 46 bytes at offset 10 are extracted
TEST_F(BbdoInputBufferExtract, Offset10Size46) {
  std::string output;
  _buffer.extract(output, 10, 46);
  ASSERT_EQ(output.size(), 46u);
  ASSERT_EQ(memcmp(output.data(), _raw.data() + 10, 46), 0);
}

TEST_F(BbdoInputBufferExtract, Erase2Offset2Offset10Size46) {
  for (int i = 0; i < 20; i++) {
    std::string output;
    _buffer.extract(output, 10, 46);
    ASSERT_EQ(output.size(), 46u);
    ASSERT_EQ(memcmp(output.data(), _raw.data() + 10 + i, 46), 0);
    _buffer.erase(1);
  }
}
