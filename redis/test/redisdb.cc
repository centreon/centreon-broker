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
    _db.reset(new redisdb("127.0.0.1", 6379, "p@ssw0rd", 10));
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
  _db->set("toto", 12);
  _db->incr("toto");
  QByteArray& res(_db->send(true));
  ASSERT_TRUE(res == "+OK\r\n:13\r\n");
}

TEST_F(RedisdbTest, SetGetKey) {
  _db->clear();
  _db->set("toto", 14);
  _db->get("toto");
  QByteArray& res(_db->send(true));
  ASSERT_TRUE(strcmp(res.constData(), "+OK\r\n$2\r\n14\r\n") == 0);
}

TEST_F(RedisdbTest, HMSetKey) {
  _db->clear();
  _db->hmset("tete", 2);
  *_db << "titi" << 25 << "tata" << 12;
  _db->hget("tete", "titi");
  QByteArray& res(_db->send(true));
  ASSERT_TRUE(strcmp(res.constData(), "+OK\r\n$2\r\n25\r\n") == 0);
}

TEST_F(RedisdbTest, HostStatus) {
  _db->unlink("h:28");
  neb::host_status hst;
  hst.host_id = 28;
  hst.current_state = 2;
  hst.enabled = true;
  hst.acknowledged = false;
  hst.check_type = 7;
  hst.next_check = 23;
  hst.state_type = 1;
  _db->push(hst);
  int row(_db->hgetall("h:28"));
  QVariantList res2(_db->parse(_db->send(true)));
  QVariantList lst(res2[row].toList());
  ASSERT_GE(lst.size(), 14);
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
  // We define two acl groups containing the host 28 ; their ideas are 3 and 5.
  _db->del("aclh:3");
  _db->del("aclh:5");

  _db->setbit("aclh:3", 28, 1);
  _db->setbit("aclh:5", 28, 1);
  neb::host hst;
  hst.host_id = 28;
  hst.host_name = "host test";
  hst.poller_id = 113;
  _db->push(hst);
  _db->hgetall("h:28");
  QVariant var(_db->parse(_db->send(true)).back());
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
  _db->unlink("s:28:42");
  neb::service svc;
  svc.host_id = 28;
  svc.service_id = 42;
  _db->push(svc);

  int row(_db->sismember("services:28", "s:28:42"));
  int res(_db->parse(_db->send(true))[row].toInt());
  ASSERT_EQ(res, 1);
}

TEST_F(RedisdbTest, ServiceStatus) {
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
  _db->hgetall("s:28:42");
  QVariant var(_db->send(true));
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
  int row(_db->sismember("hg:37", "28"));
  QVariant res(_db->parse(_db->send(true))[row]);
  ASSERT_TRUE(res.toInt() == 1);
}

TEST_F(RedisdbTest, ServiceGroupMember) {
  neb::service_group_member sgm;
  sgm.host_id = 28;
  sgm.service_id = 42;
  sgm.group_id = 68;
  _db->push(sgm);
  _db->send(true);
  int row(_db->smembers("sg:68"));
  QVariant res(_db->parse(_db->send(true))[row]);
  QVariantList lst(res.toList());
  ASSERT_TRUE(lst.size() == 1);
  ASSERT_TRUE(
    strcmp(lst[0].toByteArray().constData(),
    "s:28:42") == 0);
}

//TEST_F(RedisdbTest, ServiceGroupMemberAcl) {
//  // Let's add an acl on service group 71
//  _db->setbit("aclsg:189", 71, 1);
//
//  // Let's create the service group 71
//  neb::service_group_member sgm;
//  sgm.host_id = 28;
//  sgm.service_id = 42;
//  sgm.group_id = 71;
//  _db->push(sgm);
//  int row(_db->smembers("sg:71"));
//  QVariantList ret(redisdb::parse(_db->send(true))[row].toList());
//  ASSERT_TRUE(ret.size() == 1);
//  ASSERT_TRUE(strcmp(ret[0].toByteArray().constData(), "s:28:42") == 0);
//
//  int row1(_db->hget("s:28:42", "service_groups"));
//  ASSERT_TRUE(strcmp(redisdb::parse(_db->send(true))[row1].toByteArray().constData(), "68,71,") == 0);
//}

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
    _db->push(h);
  }
  for (i = 0; i < 10000; ++i) {
    neb::service s;
    s.host_id = (i % 10) + 1;
    s.service_id = i + 1;
    s.service_description = QString("Description%1").arg(s.service_id);
    _db->push(s);
  }
  ASSERT_NO_THROW(
  for (i = 0; i < 10000; ++i) {
    neb::service_status ss;
    ss.host_id = (i % 10) + 1;
    ss.service_id = i + 1;
    ss.service_description = QString("Description%1").arg(ss.service_id);
    ss.current_state = rand() % 4;
    _db->push(ss);
  }
  _db->send(true);
  );
}
