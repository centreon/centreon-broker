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
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/redis/redisdb.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

class RedisdbTest : public ::testing::Test {
 public:
  RedisdbTest()
    : _db("127.0.0.1", 6379, "user", "password") {}

  void SetUp() {
    try {
      config::applier::init();
    }
    catch (std::exception const& e) {
      (void) e;
    }
  }

  void TearDown() {
    config::applier::deinit();
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

TEST_F(RedisdbTest, MultiWordString2) {
  _db.clear();
  _db << "test1         test2";
  ASSERT_EQ(_db.str(), "$19\r\ntest1         test2\r\n");
}

TEST_F(RedisdbTest, Integer) {
  _db.clear();
  _db << 12;
  ASSERT_EQ(_db.str(), "$2\r\n12\r\n");
}

TEST_F(RedisdbTest, SetKey) {
  _db.clear();
  _db << "set" << "toto" << 12;
  std::cout << "str: " << _db.str() << std::endl;
  QString& res1(_db.send_command());
  std::cout << "res: " << res1.toStdString() << std::endl;
  ASSERT_TRUE(res1 == "+OK\r\n");
  _db << "incr" << "toto";
  QString& res2(_db.send_command());
  ASSERT_TRUE(res2 == ":13\r\n");
}

TEST_F(RedisdbTest, SetGetKey) {
  _db.clear();
  _db << "set" << "toto" << 14;
  std::cout << "str: " << _db.str() << std::endl;
  QString& res1(_db.send_command());
  std::cout << "res: " << res1.toStdString() << std::endl;
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "get" << "toto";
  QString& res2(_db.send_command());
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$2\r\n14\r\n");
}

TEST_F(RedisdbTest, MSetKey) {
  _db.clear();
  _db << "toto" << 14 << "titi" << 25;
  std::cout << "str: " << _db.str() << std::endl;
  QString& res1(_db.mset());
  std::cout << "res: " << res1.toStdString() << std::endl;
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "get" << "titi";
  QString& res2(_db.send_command());
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$2\r\n25\r\n");
}

TEST_F(RedisdbTest, HostStatus) {
  _db.clear();
  neb::host_status hst;
  hst.acknowledged = false;
  hst.host_id = 28;
  hst.check_type = 7;
  hst.next_check = 23;
  hst.state_type = 1;
  hst.current_state = 2;
  _db << hst;
  QString& res1(_db.mset());
  std::cout << "res: " << res1.toStdString() << std::endl;
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "get" << "28:current_state";
  QString& res2(_db.send_command());
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$1\r\n2\r\n");
  _db << "get" << "28:check_type";
  res2 = _db.send_command();
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$1\r\n7\r\n");
  _db << "get" << "28:next_check";
  res2 = _db.send_command();
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$2\r\n23\r\n");
  _db << "get" << "28:state_type";
  res2 = _db.send_command();
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$1\r\n1\r\n");
}

TEST_F(RedisdbTest, ServiceStatus) {
  _db.clear();
  neb::service_status svc;
  svc.service_id = 42;
  svc.host_id = 28;
  svc.check_type = 7;
  svc.next_check = 23;
  svc.state_type = 1;
  svc.current_state = 3;
  _db << svc;
  QString& res1(_db.mset());
  std::cout << "res: " << res1.toStdString() << std::endl;
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "get" << "28:42:current_state";
  QString& res2(_db.send_command());
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$1\r\n3\r\n");
  _db << "get" << "28:42:check_type";
  res2 = _db.send_command();
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$1\r\n7\r\n");
  _db << "get" << "28:42:next_check";
  res2 = _db.send_command();
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$2\r\n23\r\n");
  _db << "get" << "28:42:state_type";
  res2 = _db.send_command();
  std::cout << "res: " << res2.toStdString() << std::endl;
  ASSERT_TRUE(res2 == "$1\r\n1\r\n");
}
