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

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <memory>
#include <gtest/gtest.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/lua/luabinding.hh"
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::lua;

#define FILE1 CENTREON_BROKER_LUA_SCRIPT_PATH "/test1.lua"
#define FILE2 CENTREON_BROKER_LUA_SCRIPT_PATH "/test2.lua"
#define FILE3 CENTREON_BROKER_LUA_SCRIPT_PATH "/test3.lua"
#define FILE4 CENTREON_BROKER_LUA_SCRIPT_PATH "/socket.lua"

class LuaGenericTest : public ::testing::Test {
 public:
  void SetUp() {
    try {
      config::applier::init();
    }
    catch (std::exception const& e) {
      (void) e;
    }
    misc::shared_ptr<persistent_cache> pcache
      = new persistent_cache("/tmp/broker_test_cache");
    _cache.reset(new macro_cache(pcache));
  }
  void TearDown() {
    // The cache must be destroyed before the applier deinit() call.
    _cache.reset();
    config::applier::deinit();
  }

  void CreateScript(std::string const& filename, QString const& content) {
    QFile file(filename.c_str());
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << content;
  }

  QStringList ReadFile(QString const& filename) {
    QStringList retval;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
      return retval;

    QTextStream in(&file);

    while (!in.atEnd()) {
      retval << in.readLine();
    }

    file.close();
    return retval;
  }

  void RemoveFile(std::string const& filename) {
    QFile file(filename.c_str());
    file.remove();
  }

 protected:
  std::auto_ptr<macro_cache> _cache;
};

// When a lua script that does not exist is loaded
// Then an exception is thrown
TEST_F(LuaGenericTest, MissingScript) {
  QMap<QString, QVariant> conf;
  ASSERT_THROW(new luabinding(FILE1, conf, *_cache.get()), exceptions::msg);
}

// When a lua script with error such as number divided by nil is loaded
// Then an exception is thrown
TEST_F(LuaGenericTest, FaultyScript) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/faulty.lua");
  CreateScript(filename, "local a = { 1, 2, 3 }\n"
                         "local b = 18 / a[4]");
  ASSERT_THROW(
    new luabinding(filename, conf, *_cache.get()),
    exceptions::msg);
  RemoveFile(filename);
}

// When a lua script that does not contain an init() function is loaded
// Then an exception is thrown
TEST_F(LuaGenericTest, WithoutInit) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/without_init.lua");
  CreateScript(filename, "local a = { 1, 2, 3 }\n");
  ASSERT_THROW(
    new luabinding(filename, conf, *_cache.get()), exceptions::msg);
  RemoveFile(filename);
}

// When a lua script that does not contain a filter() function is loaded
// Then has_filter() method returns false
TEST_F(LuaGenericTest, WithoutFilter) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/without_filter.lua");
  CreateScript(filename, "function init()\n"
                         "end\n"
                         "function write(d)\n"
                         "  return 1\n"
                         "end");
  std::auto_ptr<luabinding> bb(
    new luabinding(filename, conf, *_cache.get()));
  ASSERT_FALSE(bb->has_filter());
  RemoveFile(filename);
}

// When a json parameters file exists but the lua script is incomplete
// Then an exception is thrown
TEST_F(LuaGenericTest, IncompleteScript) {
  QMap<QString, QVariant> conf;
  ASSERT_THROW(new luabinding(FILE2, conf, *_cache.get()), exceptions::msg);
}

// When a script is correctly loaded and a neb event has to be sent
// Then this event is translated into a Lua table and sent to the lua write()
// function.
TEST_F(LuaGenericTest, SimpleScript) {
  RemoveFile("/tmp/test.log");
  QMap<QString, QVariant> conf;
  conf["address"] = "127.0.0.1";
  conf["port"] = 8857;
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::auto_ptr<luabinding> bnd(new luabinding(FILE3, conf, *_cache.get()));
  ASSERT_TRUE(bnd.get());
  std::auto_ptr<neb::service> s(new neb::service);
  s->host_id = 12;
  s->service_id = 18;
  s->output = "Bonjour";
  misc::shared_ptr<io::data> svc(s.release());
  bnd->write(svc);

  QStringList result(ReadFile("/tmp/test.log"));
  ASSERT_EQ(result.size(), 74);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: address => 127\\.0\\.0\\.1")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: port => 8857")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: host_id => 12")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: output => Bonjour")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: service_id => 18")) >= 0);

  l.unload();
}

// When a script is correctly loaded and a neb event has to be sent
// Then this event is translated into a Lua table and sent to the lua write()
// function.
TEST_F(LuaGenericTest, WriteAcknowledgement) {
  RemoveFile("/tmp/test.log");
  QMap<QString, QVariant> conf;
  conf["address"] = "127.0.0.1";
  conf["double"] = 3.14159265358979323846;
  conf["port"] = 8857;
  conf["name"] = "test-centreon";
  modules::loader l;
  l.load_file("./neb/10-neb.so");

  std::auto_ptr<luabinding> bnd(new luabinding(FILE3, conf, *_cache.get()));
  ASSERT_TRUE(bnd.get());
  std::auto_ptr<neb::acknowledgement> s(new neb::acknowledgement);
  s->host_id = 13;
  s->author = "testAck";
  s->service_id = 21;
  misc::shared_ptr<io::data> svc(s.release());
  bnd->write(svc);

  QStringList result(ReadFile("/tmp/test.log"));
  ASSERT_EQ(result.size(), 15);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: address => 127\\.0\\.0\\.1")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: double => 3.1415926535898")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: port => 8857")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: name => test-centreon")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: host_id => 13")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: author => testAck")) >= 0);
  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: service_id => 21")) >= 0);

  l.unload();
}

// When a script is loaded and a new socket is created
// Then it is created.
TEST_F(LuaGenericTest, SocketCreation) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  local socket = broker_tcp_socket.new()\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n\n");
  ASSERT_NO_THROW(new luabinding(filename, conf, *_cache.get()));
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to connect is made without argument
// Then it fails.
TEST_F(LuaGenericTest, SocketConnectionWithoutArg) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  local socket = broker_tcp_socket.new()\n"
                         "  socket:connect()\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n\n");
  ASSERT_THROW(new luabinding(filename, conf, *_cache.get()), std::exception);
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to connect is made without argument
// Then it fails.
TEST_F(LuaGenericTest, SocketConnectionWithNoPort) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  local socket = broker_tcp_socket.new()\n"
                         "  socket:connect('127.0.0.1')\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n\n");
  ASSERT_THROW(new luabinding(filename, conf, *_cache.get()), std::exception);
  RemoveFile(filename);
}

#if 0
// Thoses tests need a little server working on 127.0.0.1:9200

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaGenericTest, SocketConnectionOk) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  local socket = broker_tcp_socket.new()\n"
                         "  socket:connect('127.0.0.1', 9200)\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n\n");
  ASSERT_NO_THROW(new luabinding(filename, conf, *_cache.get()));
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaGenericTest, SocketWrite) {
  QMap<QString, QVariant> conf;
  std::string filename(FILE4);
  ASSERT_NO_THROW(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));
  ASSERT_TRUE(lst.size() > 0);
  //RemoveFile("/tmp/log");
}
#endif

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaGenericTest, JsonEncode) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  broker_log:info('coucou')\n"
                         "  local a = { aa='bonjour',bb=12,cc={'a', 'b', 'c', 4},dd=true}\n"
                         "  local json = broker.json_encode(a)\n"
                         "  local b = broker.json_decode(json)\n"
                         "  for i,v in pairs(b) do\n"
                         "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
                         "  end"
                         "  broker_log:info(1, json)\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: aa=>bonjour")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: bb=>12")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: cc=>table: .*")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: dd=>true")) != -1);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given an empty array,
// Then json_encode() works well on it.
TEST_F(LuaGenericTest, EmptyJsonEncode) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local a = {}\n"
                         "  local json = broker.json_encode(a)\n"
                         "  broker_log:info(1, 'empty array: ' .. json)\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("INFO: empty array: []"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaGenericTest, JsonEncodeEscape) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local a = { 'bonjour le \"monde\"', 12, true, 27.1, {a=1, b=2, c=3, d=4}, 'une tabulation\\t...'}\n"
                         "  local json = broker.json_encode(a)\n"
                         "  local b = broker.json_decode(json)\n"
                         "  for i,v in ipairs(b) do\n"
                         "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
                         "  end"
                         "  broker_log:info(1, json)\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 1=>bonjour le \"monde\"")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 2=>12")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 3=>true")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 4=>27.1")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 5=>table: .*")) != -1);
  ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 6=>une tabulation\t...")) != -1);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache does not know about it
// Then nil is returned from the lua method.
TEST_F(LuaGenericTest, CacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hst = broker_cache:get_hostname(1)\n"
                         "  if not hst then\n"
                         "    broker_log:info(1, 'host does not exist')\n"
                         "  end\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("host does not exist"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache knows about it
// Then the hostname is returned from the lua method.
TEST_F(LuaGenericTest, HostCacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->host_name = strdup("centreon");
  _cache->write(hst);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hst = broker_cache:get_hostname(1)\n"
                         "  broker_log:info(1, 'host is ' .. tostring(hst))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("host is centreon"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache knows about it
// Then the hostname is returned from the lua method.
TEST_F(LuaGenericTest, ServiceCacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 14;
  svc->service_description = strdup("description");
  _cache->write(svc);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local svc = broker_cache:get_service_description(1, 14)\n"
                         "  broker_log:info(1, 'service description is ' .. tostring(svc))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("service description is description"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache knows about it
// Then the hostname is returned from the lua method.
TEST_F(LuaGenericTest, IndexMetricCacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 14;
  svc->service_description = strdup("MyDescription");
  _cache->write(svc);
  shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->host_name = strdup("host1");
  _cache->write(hst);
  shared_ptr<storage::index_mapping> im(new storage::index_mapping);
  im->index_id = 7;
  im->service_id = 14;
  im->host_id = 1;
  _cache->write(im);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local index_mapping = broker_cache:get_index_mapping(7)\n"
                         "  local svc = broker_cache:get_service_description(index_mapping.host_id, index_mapping.service_id)\n"
                         "  broker_log:info(1, 'service description is ' .. tostring(svc))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("service description is MyDescription"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for an instance is made
// And the cache knows about it
// Then the instance is returned from the lua method.
TEST_F(LuaGenericTest, InstanceNameCacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<instance_broadcast> ib(new instance_broadcast);
  ib->broker_id = 42;
  ib->broker_name = "broker name";
  ib->enabled = true;
  ib->poller_id = 18;
  ib->poller_name = "MyPoller";
  _cache->write(ib);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local instance_name = broker_cache:get_instance_name(18)\n"
                         "  broker_log:info(1, 'instance name is ' .. tostring(instance_name))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("instance name is MyPoller"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a metric mapping is made
// And the cache knows about it
// Then the metric mapping is returned from the lua method.
TEST_F(LuaGenericTest, MetricMappingCacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<storage::metric_mapping> mm(new storage::metric_mapping);
  mm->index_id = 19;
  mm->metric_id = 27;
  _cache->write(mm);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local mm = broker_cache:get_metric_mapping(27)\n"
                         "  broker_log:info(1, 'metric id is ' .. mm.metric_id)\n"
                         "  broker_log:info(1, 'index id is ' .. mm.index_id)\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("metric id is 27"));
  ASSERT_TRUE(lst[1].contains("index id is 19"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a host group name is made
// And the cache does not know about it
// Then nil is returned by the lua method.
TEST_F(LuaGenericTest, HostGroupCacheTestNameNotAvailable) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hg = broker_cache:get_hostgroup_name(28)\n"
                         "  broker_log:info(1, 'host group is ' .. tostring(hg))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("host group is nil"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a host group name is made
// And the cache does know about it
// Then the name is returned by the lua method.
TEST_F(LuaGenericTest, HostGroupCacheTestName) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<neb::host_group> hg(new neb::host_group);
  hg->id = 28;
  hg->name = strdup("centreon");
  _cache->write(hg);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hg = broker_cache:get_hostgroup_name(28)\n"
                         "  broker_log:info(1, 'host group is ' .. tostring(hg))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("host group is centreon"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for host groups is made
// And the host is attached to no group
// Then an empty array is returned.
TEST_F(LuaGenericTest, HostGroupCacheTestEmpty) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hg = broker_cache:get_hostgroups(1)\n"
                         "  broker_log:info(1, 'host group is ' .. broker.json_encode(hg))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("host group is []"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for host groups is made
// And the cache does know about them
// Then an array is returned by the lua method.
TEST_F(LuaGenericTest, HostGroupCacheTest) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<neb::host_group> hg(new neb::host_group);
  hg->id = 16;
  hg->name = strdup("centreon1");
  _cache->write(hg);
  hg = new neb::host_group;
  hg->id = 17;
  hg->name = strdup("centreon2");
  _cache->write(hg);
  shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 22;
  hst->host_name = strdup("host_centreon");
  _cache->write(hst);
  shared_ptr<neb::host_group_member> member(new neb::host_group_member);
  member->host_id = 22;
  member->group_id = 16;
  _cache->write(member);
  member = new neb::host_group_member;
  member->host_id = 22;
  member->group_id = 17;
  _cache->write(member);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hg = broker_cache:get_hostgroups(22)\n"
                         "  for i,v in ipairs(hg) do\n"
                         "    broker_log:info(1, 'member of ' .. v)\n"
                         "  end\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  int first, second;
  if (lst[0].contains("member of centreon1")) {
    first = 0;
    second = 1;
  }
  else {
    first = 1;
    second = 0;
  }
  ASSERT_TRUE(lst[first].contains("member of centreon1"));
  ASSERT_TRUE(lst[second].contains("member of centreon2"));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for host groups is made
// And the cache does know about them
// And a host group name is missing
// Then nil is returned. It is a case where the cache is badly built.
TEST_F(LuaGenericTest, HostGroupCacheTestFailure) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/cache_test.lua");
  shared_ptr<neb::host_group> hg(new neb::host_group);
  hg->id = 16;
  hg->name = strdup("centreon1");
  _cache->write(hg);
  shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 22;
  hst->host_name = strdup("host_centreon");
  _cache->write(hst);
  shared_ptr<neb::host_group_member> member(new neb::host_group_member);
  member->host_id = 22;
  member->group_id = 16;
  _cache->write(member);
  member = new neb::host_group_member;
  member->host_id = 22;
  member->group_id = 17;
  _cache->write(member);

  CreateScript(filename, "function init(conf)\n"
                         "  broker_log:set_parameters(3, '/tmp/log')\n"
                         "  local hg = broker_cache:get_hostgroups(22)\n"
                         "  broker_log:info(1, 'member of ' .. tostring(hg))\n"
                         "end\n\n"
                         "function write(d)\n"
                         "end\n");
  std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
  QStringList lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst[0].contains("member of nil"));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}
