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

#include "com/centreon/broker/misc/buffer.hh"
#include <gtest/gtest.h>

using namespace com::centreon::broker::misc;

TEST(Buffer, Empty) {
  buffer buf;
  ASSERT_TRUE(buf.pop(1).empty());
}

TEST(Buffer, Append1) {
  buffer buf;
  std::vector<char> v{'a', 'b', 'c', 'd', 'e'};
  buf.push(v);
  auto vv = buf.pop(2);
  ASSERT_TRUE(memcmp(vv.data(), "ab", 2) == 0);
  ASSERT_EQ(vv.size(), 2u);
  vv = buf.pop(3);
  ASSERT_TRUE(memcmp(vv.data(), "cde", 3) == 0);
  ASSERT_EQ(vv.size(), 3u);
  ASSERT_TRUE(buf.pop(1).empty());
}

TEST(Buffer, Append2) {
  buffer buf;
  std::vector<char> v1{'a', 'b', 'c', 'd', 'e'};
  std::vector<char> v2{'f', 'g', 'h', 'i', 'j'};
  std::vector<char> v3{'k', 'l', 'm', 'n', 'o'};
  buf.push(v1);
  buf.push(v2);
  buf.push(v3);
  auto vv = buf.pop(7);
  ASSERT_TRUE(memcmp(vv.data(), "abcdefg", 7) == 0);
  ASSERT_EQ(vv.size(), 7u);
  vv = buf.pop(20);
  ASSERT_TRUE(memcmp(vv.data(), "hijklmno", 8) == 0);
  ASSERT_EQ(vv.size(), 8u);
}

TEST(Buffer, Brackets) {
  buffer buf;
  std::vector<char> v1{'a', 'b', 'c', 'd', 'e'};
  std::vector<char> v2{'f', 'g', 'h', 'i', 'j'};
  buf.push(v1);
  buf.push(v2);
  std::string str;
  for (uint32_t i = 0 ; i < buf.size(); ++i)
    str += buf[i];

  ASSERT_EQ(str, std::string("abcdefghij"));
  ASSERT_THROW(str += buf[buf.size()], std::out_of_range);
}

TEST(Buffer, BracketsDelta) {
  buffer buf;
  std::vector<char> v1{'a', 'b', 'c', 'd', 'e'};
  std::vector<char> v2{'f', 'g', 'h', 'i', 'j'};
  buf.push(v1);
  buf.push(v2);
  buf.pop(2);
  std::string str;
  for (uint32_t i = 0 ; i < buf.size(); ++i)
    str += buf[i];

  ASSERT_EQ(str, std::string("cdefghij"));
  ASSERT_THROW(str += buf[buf.size()], std::out_of_range);
}

TEST(Buffer, Empty1) {
  buffer buf;
  std::vector<char> v1{'a', 'b'};
  std::vector<char> v2{'c', 'd'};
  std::vector<char> v3{'e', 'f'};
  buf.push(v1);
  buf.push(v2);
  buf.push(v3);
  size_t len = 6;
  for (int i = 0; i < 6; i++) {
    --len;
    buf.pop(1);
    ASSERT_EQ(buf.size(), len);
  }
  ASSERT_TRUE(buf.empty());
}

TEST(Buffer, Empty2) {
  buffer buf;
  buf.push(std::vector<char>());
  buf.push(std::vector<char>());
  buf.push(std::vector<char>());
  ASSERT_TRUE(buf.empty());
}
