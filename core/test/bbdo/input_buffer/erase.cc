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
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

class BbdoInputBufferErase : public ::testing::Test {
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
// When erase(0) is called
// Then the buffer size does not change
// And the buffer content does not change
TEST_F(BbdoInputBufferErase, EraseZero) {
  // When
  _buffer.erase(0);

  // Then
  ASSERT_EQ(_buffer.size(), static_cast<int>(_raw.size()));
  std::string output;
  _buffer.extract(output, 0, _raw.size());
  ASSERT_EQ(output, _raw);
}

// Given a bbdo::input_buffer object filled with data
// When erase(1) is called
// Then the buffer size is reduced by one
// And the first byte of the buffer is removed
TEST_F(BbdoInputBufferErase, EraseOne) {
  // When
  _buffer.erase(1);

  // Then
  ASSERT_EQ(_buffer.size(), static_cast<int>(_raw.size() - 1));
  std::string output;
  _buffer.extract(output, 0, _raw.size() - 1);
  ASSERT_EQ(output, _raw.substr(1));
}

// Given a bbdo::input_buffer object filled with data
// When erase(100) is called
// Then the buffer size is reduced by 100
// And the 100 first bytes of the buffer are removed
TEST_F(BbdoInputBufferErase, EraseHundred) {
  // When
  _buffer.erase(100);

  // Then
  ASSERT_EQ(_buffer.size(), static_cast<int>(_raw.size() - 100));
  std::string output;
  _buffer.extract(output, 0, _raw.size() - 100);
  ASSERT_EQ(output, _raw.substr(100));
}

// Given a bbdo::input_buffer object filled with data
// When erase() is called with the number of bytes in the buffer
// Then the buffer size is 0
// And no data can be extracted
TEST_F(BbdoInputBufferErase, EraseAll) {
  // When
  _buffer.erase(_raw.size());

  // Then
  ASSERT_EQ(_buffer.size(), 0);
  std::string output;
  ASSERT_THROW(_buffer.extract(output, 0, 1), exceptions::msg);
}

// Given a bbdo::input_buffer object filled with data
// When erase() is called with more bytes than available in the buffer
// Then the buffer size is 0
// And no data can be extracted
TEST_F(BbdoInputBufferErase, EraseMore) {
  // When
  _buffer.erase(10000);

  // Then
  ASSERT_EQ(_buffer.size(), 0);
  std::string output;
  ASSERT_THROW(_buffer.extract(output, 0, 1), exceptions::msg);
}
