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

  void TearDown() {}

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
  QByteArray& res1(_db.push_command());
  ASSERT_TRUE(res1 == "+OK\r\n");
  _db << "incr" << "toto";
  QByteArray& res2(_db.push_command());
  ASSERT_TRUE(res2 == ":13\r\n");
}

TEST_F(RedisdbTest, SetGetKey) {
  _db.clear();
  _db << "set" << "toto" << 14;
  QByteArray& res1(_db.push_command());
  ASSERT_TRUE(strcmp(res1.constData(), "+OK\r\n") == 0);
  _db << "get" << "toto";
  QByteArray& res2(_db.push_command());
  ASSERT_TRUE(strcmp(res2.constData(), "$2\r\n14\r\n") == 0);
}

TEST_F(RedisdbTest, HMSetKey) {
  _db.clear();
  _db << "tete" << "titi" << 25 << "tata" << 12;
  QByteArray& res1(_db.push_command("$5\r\nhmset\r\n"));
  ASSERT_TRUE(strcmp(res1.constData(), "+OK\r\n") == 0);
  _db << "hget" << "tete" << "titi";
  QByteArray& res2(_db.push_command());
  ASSERT_TRUE(strcmp(res2.constData(), "$2\r\n25\r\n") == 0);
}

TEST_F(RedisdbTest, HostStatus) {
  _db.clear();
  _db << "h:28";
  _db.del();
  neb::host_status hst;
  hst.host_id = 28;
  hst.current_state = 2;
  hst.enabled = true;
  hst.acknowledged = false;
  hst.check_type = 7;
  hst.next_check = 23;
  hst.state_type = 1;
  QByteArray& res1(_db.push(hst));
  ASSERT_TRUE(strcmp(res1.constData(), "+OK\r\n") == 0);
  _db << "hgetall" << "h:28";
  QByteArray& vv(_db.push_command());
  std::cout << "vv = " << vv.constData() << std::endl;
  QVariant res2(redisdb::parse(vv));
  QVariantList lst(res2.toList());
  ASSERT_EQ(lst.size(), 14);
  for (int i = 0; i < lst.size(); i += 2) {
    if (lst[i] == "current_state") {
      ASSERT_EQ(lst[i + 1], 2);
    }
    else if (lst[i] == "enabled") {
      ASSERT_EQ(lst[i + 1], 1);
    }
    else if (lst[i] == "acknowledged") {
      ASSERT_EQ(lst[i + 1], 0);
    }
    else if (lst[i] == "check_type") {
      ASSERT_EQ(lst[i + 1], 7);
    }
    else if (lst[i] == "next_check") {
      ASSERT_EQ(lst[i + 1], 23);
    }
    else if (lst[i] == "state_type") {
      ASSERT_EQ(lst[i + 1], 1);
    }
  }
}

TEST_F(RedisdbTest, HostWithNameStatus) {
  _db.clear();
  neb::host hst;
  hst.host_id = 28;
  hst.host_name = "host test";
  hst.poller_id = 113;
  QByteArray& res1(_db.push(hst));
  std::cout << "res1 = " << res1.constData() << std::endl;
  ASSERT_TRUE(strcmp(res1.constData(), ":0\r\n") == 0
              || strcmp(res1.constData(), ":1\r\n") == 0);
  _db << "hgetall" << "h:28";
  QByteArray& res2(_db.push_command());
  std::cout << "res2 = " << res2.constData() << std::endl;
  QVariant var(redisdb::parse(res2));
  QVariantList lst(var.toList());
  ASSERT_EQ(lst.size(), 30);
  for (int i = 0; i < lst.size(); i += 2) {
    if (lst[i] == "name") {
      ASSERT_EQ(lst[i + 1], "host test");
    }
    else if (lst[i] == "poller_id") {
      ASSERT_EQ(lst[i + 1], 113);
    }
  }
}

TEST_F(RedisdbTest, ServiceStatus) {
  _db.clear();
  _db << "s:28:42";
  _db.del();
  neb::service_status svc;
  svc.host_id = 28;
  svc.service_id = 42;
  svc.current_state = 3;
  svc.enabled = true;
  svc.acknowledged = true;
  svc.check_type = 7;
  svc.next_check = 23;
  svc.state_type = 1;
  QByteArray& res1(_db.push(svc));
  ASSERT_TRUE(strcmp(res1.constData(), "+OK\r\n") == 0);
  _db << "hgetall" << "s:28:42";
  QByteArray& res2(_db.push_command());
  QVariant var(redisdb::parse(res2));
  QVariantList lst(var.toList());

  for (int i = 0; i < lst.size(); i += 2) {
    if (lst[i] == "name") {
      ASSERT_EQ(lst[i + 1], "host test");
    }
    else if (lst[i] == "poller_id") {
      ASSERT_EQ(lst[i + 1], 113);
    }
  }
}

TEST_F(RedisdbTest, HostGroupMember) {
  neb::host_group_member hgm;
  hgm.host_id = 28;
  hgm.group_id = 37;
  _db.push(hgm);
  _db << "hg:28";
  QByteArray ret(_db.push_command("$3\r\nGET\r\n"));
  QVariant res(redisdb::parse(ret));
  std::string str(redisdb::parse_bitfield(res.toByteArray()));
  std::cout << "flags: " << str << std::endl;
  ASSERT_TRUE(str == "37,");
}
