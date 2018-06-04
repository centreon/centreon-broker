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
#include <memory>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/redis/redisdb.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

class RedisdbTest : public ::testing::Test {
 public:

  void SetUp() {
    try {
      config::applier::init();
    }
    catch (std::exception const& e) {
      (void) e;
    }
    _db.reset(new redisdb("127.0.0.1", 6379, "p@ssw0rd"));
  }

  void TearDown() {
    _db.reset();
    config::applier::deinit();
  }

 protected:
  std::auto_ptr<redisdb> _db;
};

// When a one word string is append to the redisdb
// Then the word is well understood and the str() method is able to return it.
TEST_F(RedisdbTest, OneWordString) {
  _db->clear();
  *_db << "test";
  ASSERT_EQ(_db->str(), "$4\r\ntest\r\n");
}

TEST_F(RedisdbTest, MultiWordString2) {
  _db->clear();
  *_db << "test1         test2";
  ASSERT_EQ(_db->str(), "$19\r\ntest1         test2\r\n");
}

TEST_F(RedisdbTest, Integer) {
  _db->clear();
  *_db << 12;
  ASSERT_EQ(_db->str(), "$2\r\n12\r\n");
}

TEST_F(RedisdbTest, SetKey) {
  _db->clear();
  *_db << "set" << "toto" << 12;
  QByteArray& res1(_db->push_command());
  ASSERT_TRUE(res1 == "+OK\r\n");
  *_db << "incr" << "toto";
  QByteArray& res2(_db->push_command());
  ASSERT_TRUE(res2 == ":13\r\n");
}

TEST_F(RedisdbTest, SetGetKey) {
  _db->clear();
  *_db << "set" << "toto" << 14;
  QByteArray& res1(_db->push_command());
  ASSERT_TRUE(strcmp(res1.constData(), "+OK\r\n") == 0);
  *_db << "get" << "toto";
  QByteArray& res2(_db->push_command());
  ASSERT_TRUE(strcmp(res2.constData(), "$2\r\n14\r\n") == 0);
}

TEST_F(RedisdbTest, HMSetKey) {
  _db->clear();
  *_db << "tete" << "titi" << 25 << "tata" << 12;
  QByteArray& res1(_db->push_command("$5\r\nhmset\r\n"));
  ASSERT_TRUE(strcmp(res1.constData(), "+OK\r\n") == 0);
  *_db << "hget" << "tete" << "titi";
  QByteArray& res2(_db->push_command());
  ASSERT_TRUE(strcmp(res2.constData(), "$2\r\n25\r\n") == 0);
}

TEST_F(RedisdbTest, HostStatus) {
  _db->clear();
  *_db << "h:28";
  _db->unlink();
  neb::host_status hst;
  hst.host_id = 28;
  hst.current_state = 2;
  hst.enabled = true;
  hst.acknowledged = false;
  hst.check_type = 7;
  hst.next_check = 23;
  hst.state_type = 1;
  QVariant res1(_db->push(hst));
  std::cout << "HOST STATUS OUTPUT " << res1.toByteArray().constData() << std::endl;
  ASSERT_TRUE(strcmp(res1.toByteArray().constData(), "+OK") == 0);
  *_db << "h:28";
  QVariant res2(_db->hgetall());
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
  _db->clear();
  // We define two acl groups containing the host 28 ; their ideas are 3 and 5.
  *_db << "aclh:3" << "aclh:5";
  _db->del();

  *_db << "aclh:3" << 28 << 1;
  _db->setbit();
  *_db << "aclh:5" << 28 << 1;
  _db->setbit();
  neb::host hst;
  hst.host_id = 28;
  hst.host_name = "host test";
  hst.poller_id = 113;
  int res1(_db->push(hst).toInt());
  ASSERT_TRUE(res1 == 0 || res1 == 1);
  *_db << "h:28";
  QVariant var(_db->hgetall());
  QVariantList lst(var.toList());
  ASSERT_EQ(lst.size(), 30);
  for (int i = 0; i < lst.size(); i += 2) {
    if (lst[i] == "name") {
      ASSERT_EQ(lst[i + 1], "host test");
    }
    else if (lst[i] == "poller_id") {
      ASSERT_EQ(lst[i + 1], 113);
    }
    else if (lst[i] == "acl_groups") {
      char const* content(lst[i + 1].toByteArray().constData());
      ASSERT_TRUE(strcmp(content, "5,3,") == 0
                  || strcmp(content, "3,5,") == 0);
    }
  }
}

TEST_F(RedisdbTest, Service) {
  _db->clear();
  *_db << "s:28:42";
  _db->unlink();
  neb::service svc;
  svc.host_id = 28;
  svc.service_id = 42;
  _db->push(svc);

  *_db << "services:28" << "s:28:42";
  int res(_db->sismember().toInt());
  ASSERT_EQ(res, 1);
}

TEST_F(RedisdbTest, ServiceStatus) {
  _db->clear();
  *_db << "s:28:42";
  neb::service_status svc;
  svc.host_id = 28;
  svc.service_id = 42;
  svc.current_state = 3;
  svc.enabled = true;
  svc.acknowledged = true;
  svc.check_type = 7;
  svc.next_check = 23;
  svc.state_type = 1;
  _db->push(svc);
  *_db << "s:28:42";
  QVariant var(_db->hgetall());
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
  _db->push(hgm);
  *_db << "hg:37";
  QVariant res(_db->get());
  std::string str(redisdb::parse_bitfield(res.toByteArray()));
  ASSERT_TRUE(str == "28,");
}

TEST_F(RedisdbTest, ServiceGroupMember) {
  neb::service_group_member sgm;
  sgm.host_id = 28;
  sgm.service_id = 42;
  sgm.group_id = 68;
  _db->push(sgm);
  *_db << "SMEMBERS" << "sg:68";
  QByteArray ret(_db->push_command());
  QVariant res(redisdb::parse(ret));
  ASSERT_TRUE(res.toList().size() == 1);
  ASSERT_TRUE(
    strcmp(res.toList()[0].toByteArray().constData(),
    "s:28:42") == 0);
}

TEST_F(RedisdbTest, ServiceGroupMemberAcl) {
  // Let's add an acl on service group 71
  *_db << "aclsg:189" << 71 << 1;
  _db->setbit();

  // Let's create the service group 71
  neb::service_group_member sgm;
  sgm.host_id = 28;
  sgm.service_id = 42;
  sgm.group_id = 71;
  _db->push(sgm);
  *_db << "SMEMBERS" << "sg:71";
  QByteArray ret(_db->push_command());
  QVariant res(redisdb::parse(ret));
  ASSERT_TRUE(res.toList().size() == 1);
  ASSERT_TRUE(strcmp(res.toList()[0].toByteArray().constData(), "s:28:42") == 0);

  *_db << "s:28:42" << "service_groups";
  res = _db->hget();
  ASSERT_TRUE(strcmp(res.toByteArray().constData(), "68,71,") == 0);
}

TEST_F(RedisdbTest, ManyServices) {
  int i;
  // Let's create 10 hosts
  for (i = 0; i < 10; ++i) {
    neb::host h;
    h.host_id = i + 1;
    std::ostringstream oss;
    oss << "Host" << h.host_id;
    h.host_name = QString("Host%1").arg(h.host_id);
    h.poller_id = 113;
    int res(_db->push(h).toInt());
    ASSERT_TRUE(res == 0 || res == 1);
  }
  for (i = 0; i < 10000; ++i) {
    neb::service s;
    s.host_id = (rand() % 10) + 1;
    s.service_id = i + 1;
    s.service_description = QString("Description%1").arg(s.service_id);
    int res(_db->push(s).toInt());
    ASSERT_TRUE(res == 0 || res == 1);
  }
}
