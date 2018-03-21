/*
** Copyright 2018 Centreon
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
#include "com/centreon/broker/redis/redisdb.hh"

using namespace com::centreon::broker::redis;

class RedisdbTest : public ::testing::Test {
 public:
  void SetUp() {
  }

  void TearDown() {
  }

 protected:
  redisdb _db;
};

// When a one word string is append to the redisdb
// Then the word is well understood and the str() method is able to return it.
TEST_F(RedisdbTest, OneWordString) {
  _db.clear();
  _db << "test";
  ASSERT_EQ(_db.str(), "$4\r\ntest\r\n");
}

TEST_F(RedisdbTest, MultiWordString) {
  _db.clear();
  _db << "test1 test2";
  ASSERT_EQ(_db.str(), "*2\r\n$5\r\ntest1\r\n$5\r\ntest2\r\n");
}

TEST_F(RedisdbTest, MultiWordString2) {
  _db.clear();
  _db << "test1         test2";
  ASSERT_EQ(_db.str(), "*2\r\n$5\r\ntest1\r\n$5\r\ntest2\r\n");
}

TEST_F(RedisdbTest, Integer) {
  _db.clear();
  _db << 12;
  ASSERT_EQ(_db.str(), ":12\r\n");
}

TEST_F(RedisdbTest, SetKey) {
  _db.clear();
  _db << "set" << "toto" << 12;
  _db.flush();
  _db << "incr" << "toto";
}
