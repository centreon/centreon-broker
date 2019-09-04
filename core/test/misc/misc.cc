/*
** Copyright 2019 Centreon
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
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker::misc;

TEST(MiscTest, SimpleFromHex) {
  std::string const str = "0102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20";
  std::vector<char> v{from_hex(str)};
  char value = 1;
  for (char c : v) {
    ASSERT_EQ(c, value);
    value++;
  }
}

TEST(MiscTest, SimpleLowerFromHex) {
  std::string const str = "a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0";
  std::vector<char> v{from_hex(str)};
  char value = 0xa1;
  for (char c : v) {
    ASSERT_EQ(c, value);
    value++;
  }
}

TEST(MiscTest, BadCharFromHex) {
  std::string const str = "Hollow";
  ASSERT_THROW(from_hex(str), std::exception);
}

TEST(MiscTest, BadLengthFromHex) {
  std::string const str = "abcde";
  ASSERT_THROW(from_hex(str), std::exception);
}
