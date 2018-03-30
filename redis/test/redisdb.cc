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
    : _db("127.0.0.1", 6379, "p@ssw0rd") {}

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
  _db.push_command();
  QString& res1(_db.flush());
  ASSERT_TRUE(res1 == "+OK\r\n");
  _db << "incr" << "toto";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == ":13\r\n");
}

TEST_F(RedisdbTest, SetGetKey) {
  _db.clear();
  _db << "set" << "toto" << 14;
  _db.push_command();
  QString& res1(_db.flush());
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "get" << "toto";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == "$2\r\n14\r\n");
}

TEST_F(RedisdbTest, HMSetKey) {
  _db.clear();
  _db << "tete" << "titi" << 25 << "tata" << 12;
  _db.hmset();
  QString& res1(_db.flush());
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "hget" << "tete" << "titi";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == "$2\r\n25\r\n");
}

TEST_F(RedisdbTest, HostStatus) {
  _db.clear();
  _db << "h:28";
  _db.del();
  _db.flush();
  neb::host_status hst;
  hst.host_id = 28;
  hst.current_state = 2;
  hst.enabled = true;
  hst.acknowledged = false;
  hst.check_type = 7;
  hst.next_check = 23;
  hst.state_type = 1;
  _db.push(hst);
  QString& res1(_db.flush());
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "hgetall" << "h:28";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == "*6\r\n$5\r\nstate\r\n$1\r\n2\r\n$7\r\nenabled\r\n$1\r\n1\r\n$12\r\nacknowledged\r\n$1\r\n0\r\n");
}

TEST_F(RedisdbTest, HostWithNameStatus) {
  _db.clear();
  ASSERT_THROW(_db.flush(), std::exception);
  neb::host hst;
  hst.host_id = 28;
  hst.host_name = "host test";
  _db.push(hst);
  QString& res1(_db.flush());
  ASSERT_TRUE(res1 == ":1\r\n");
  _db << "hgetall" << "h:28";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == "*8\r\n$5\r\nstate\r\n$1\r\n2\r\n$7\r\nenabled\r\n$1\r\n1\r\n$12\r\nacknowledged\r\n$1\r\n0\r\n$4\r\nname\r\n$9\r\nhost test\r\n");
}

TEST_F(RedisdbTest, ServiceStatus) {
  _db.clear();
  _db << "s:28:42";
  _db.del();
  _db.flush();
  neb::service_status svc;
  svc.host_id = 28;
  svc.service_id = 42;
  svc.current_state = 3;
  svc.enabled = true;
  svc.acknowledged = true;
  svc.check_type = 7;
  svc.next_check = 23;
  svc.state_type = 1;
  _db.push(svc);
  QString& res1(_db.flush());
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n");
  _db << "hgetall" << "s:28:42";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == "*6\r\n$5\r\nstate\r\n$1\r\n3\r\n$7\r\nenabled\r\n$1\r\n1\r\n$12\r\nacknowledged\r\n$1\r\n1\r\n");
}

TEST_F(RedisdbTest, ServicesStatus) {
  _db.clear();
  neb::service svc;
  svc.host_id = 28;
  svc.service_id = 42;
  svc.service_description = "service test";
  _db.push(svc);
  QString& res(_db.flush());
  ASSERT_TRUE(res == ":1\r\n");

  neb::service_status ssvc;
  ssvc.host_id = 28;
  ssvc.service_id = 42;
  ssvc.current_state = 3;
  ssvc.enabled = true;
  ssvc.acknowledged = true;
  ssvc.check_type = 7;
  ssvc.next_check = 23;
  ssvc.state_type = 1;
  _db.push(ssvc);

  ssvc.host_id = 30;
  ssvc.service_id = 40;
  ssvc.current_state = 1;
  ssvc.enabled = true;
  ssvc.acknowledged = false;
  _db.push(ssvc);
  QString& res1(_db.flush());
  ASSERT_TRUE(res1.toStdString() == "+OK\r\n+OK\r\n");
  _db << "hgetall" << "s:28:42";
  _db.push_command();
  QString& res2(_db.flush());
  ASSERT_TRUE(res2 == "*8\r\n$5\r\nstate\r\n$1\r\n3\r\n$7\r\nenabled\r\n$1\r\n1\r\n$12\r\nacknowledged\r\n$1\r\n1\r\n$11\r\ndescription\r\n$12\r\nservice test\r\n");

  _db << "hgetall" << "s:30:40";
  _db.push_command();
  res2 = _db.flush();
  ASSERT_TRUE(res2 == "*6\r\n$5\r\nstate\r\n$1\r\n1\r\n$7\r\nenabled\r\n$1\r\n1\r\n$12\r\nacknowledged\r\n$1\r\n0\r\n");
}
