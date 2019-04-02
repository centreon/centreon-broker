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
#include "com/centreon/broker/simu/luabinding.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/modules/loader.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::simu;

class SimuGenericTest : public ::testing::Test {
 public:
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
};

// When a lua script that does not exist is loaded
// Then an exception is thrown
TEST_F(SimuGenericTest, MissingScript) {
  QMap<QString, QVariant> conf;
  ASSERT_THROW(
    new luabinding(
      "/tmp/this_script_does_not_exist.lua",conf),
		exceptions::msg);
}

// When a lua script with error such as number divided by nil is loaded
// Then an exception is thrown
TEST_F(SimuGenericTest, FaultyScript) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/faulty.lua");
  CreateScript(filename, "local a = { 1, 2, 3 }\n"
                         "local b = 18 / a[4]");
  ASSERT_THROW(
    new luabinding(filename, conf),
    exceptions::msg);
  RemoveFile(filename);
}

// When a lua script that does not contain an init() function is loaded
// Then an exception is thrown
TEST_F(SimuGenericTest, WithoutInit) {
  QMap<QString, QVariant> conf;
  std::string filename("/tmp/without_init.lua");
  CreateScript(filename, "local a = { 1, 2, 3 }\n");
  ASSERT_THROW(
    new luabinding(filename, conf), exceptions::msg);
  RemoveFile(filename);
}

// When a json parameters file exists but the lua script is incomplete
// Then an exception is thrown
TEST_F(SimuGenericTest, IncompleteScript) {
  std::string filename("/tmp/bad_init.lua");
  CreateScript(filename, "function init()\n"
                         "end\n"
                         "local a = { 1, 2, 3 }\n");
  QMap<QString, QVariant> conf;
  ASSERT_THROW(new luabinding(
                     filename,
                     conf),
                  exceptions::msg);
  RemoveFile(filename);
}

// When a script read() function does not return nil or a Lua table
// Then an exception is thrown
TEST_F(SimuGenericTest, ReadReturnValue1) {
  std::string filename("/tmp/bad_read.lua");
  CreateScript(filename, "function init()\n"
                         "end\n"
                         "function read()\n"
                         "return 2\n"
                         "end\n");
  QMap<QString, QVariant> conf;
  std::auto_ptr<luabinding> lb(new luabinding(
                     filename,
                     conf));
  misc::shared_ptr<io::data> d;
  ASSERT_THROW(lb->read(d), exceptions::msg);
  RemoveFile(filename);
}

// When a script read() function returns nil
// Then no exception is thrown
TEST_F(SimuGenericTest, ReadReturnValue2) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename, "function init()\n"
                         "end\n"
                         "function read()\n"
                         "return nil\n"
                         "end\n");
  QMap<QString, QVariant> conf;
  std::auto_ptr<luabinding> lb(new luabinding(
                     filename,
                     conf));
  misc::shared_ptr<io::data> d;
  ASSERT_FALSE(lb->read(d));
  RemoveFile(filename);
}

// When a script read() function returns a table
// And the table does not represent an event
// Then no exception is thrown
// And read() returns false
TEST_F(SimuGenericTest, ReadReturnValue3) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename, "function init()\n"
                         "end\n"
                         "function read()\n"
                         "return { a='toto' }\n"
                         "end\n");
  QMap<QString, QVariant> conf;
  std::auto_ptr<luabinding> lb(new luabinding(
                     filename,
                     conf));
  misc::shared_ptr<io::data> d;
  ASSERT_FALSE(lb->read(d));
  RemoveFile(filename);
}

// When a script read() function returns a table
// And the table is an event
// Then no exception is thrown
// And read() returns true
TEST_F(SimuGenericTest, ReadReturnValue4) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename, "function init()\n"
                         "end\n"
                         "function read()\n"
                         "return { type=65559,\n"
                                  "service_id=1,\n"
                                  "ctime=1536659255,\n"
                                  "host_id=2,\n"
                                  "element=23,\n"
                                  "category=1,\n"
                                  "description=\"Super description\"\n"
                         "}\n"
                         "end\n");
  QMap<QString, QVariant> conf;
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  std::auto_ptr<luabinding> lb(new luabinding(
                     filename,
                     conf));
  misc::shared_ptr<io::data> d;
  ASSERT_TRUE(lb->read(d));
  RemoveFile(filename);
  neb::service* svc(static_cast<neb::service*>(d.data()));
  ASSERT_TRUE(svc->type() == 65559);
  ASSERT_EQ(svc->host_id, 2);
  std::cout << "service description: " << svc->service_description.toStdString();
  ASSERT_TRUE(svc->service_description == "Super description");
}

//TEST_F(LuaGenericTest, SimpleScript) {
//  RemoveFile("/tmp/test.log");
//  QMap<QString, QVariant> conf;
//  CreateScript(filename, "function init()\n"
//                         "end\n"
//                         "function read()\n"
//                         "return { a='toto' }\n"
//                         "end\n");
//
//  std::auto_ptr<luabinding> lb(new luabinding(filename, conf, *_cache.get()));
//  ASSERT_TRUE(lb.get());
//  std::auto_ptr<neb::service> s(new neb::service);
//  s->host_id = 12;
//  s->service_id = 18;
//  s->output = "Bonjour";
//  misc::shared_ptr<io::data> svc(s.release());
//  bnd->write(svc);
//
//  QStringList result(ReadFile("/tmp/test.log"));
//  ASSERT_EQ(result.size(), 74);
//  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: address => 127\\.0\\.0\\.1")) >= 0);
//  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: port => 8857")) >= 0);
//  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: host_id => 12")) >= 0);
//  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: output => Bonjour")) >= 0);
//  ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: service_id => 18")) >= 0);
//
//  l.unload();
//}

// // When a script is correctly loaded and a neb event has to be sent
// // Then this event is translated into a Lua table and sent to the lua write()
// // function.
// TEST_F(LuaGenericTest, WriteAcknowledgement) {
//   RemoveFile("/tmp/test.log");
//   QMap<QString, QVariant> conf;
//   conf["address"] = "127.0.0.1";
//   conf["double"] = 3.14159265358979323846;
//   conf["port"] = 8857;
//   conf["name"] = "test-centreon";
//   modules::loader l;
//   l.load_file("./neb/10-neb.so");
// 
//   std::auto_ptr<luabinding> bnd(new luabinding(FILE3, conf, *_cache.get()));
//   ASSERT_TRUE(bnd.get());
//   std::auto_ptr<neb::acknowledgement> s(new neb::acknowledgement);
//   s->host_id = 13;
//   s->author = "testAck";
//   s->service_id = 21;
//   misc::shared_ptr<io::data> svc(s.release());
//   bnd->write(svc);
// 
//   QStringList result(ReadFile("/tmp/test.log"));
//   ASSERT_EQ(result.size(), 15);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: address => 127\\.0\\.0\\.1")) >= 0);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: double => 3.1415926535898")) >= 0);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: port => 8857")) >= 0);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: init: name => test-centreon")) >= 0);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: host_id => 13")) >= 0);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: author => testAck")) >= 0);
//   ASSERT_TRUE(result.indexOf(QRegExp(".*INFO: write: service_id => 21")) >= 0);
// 
//   l.unload();
// }
// 
// // When a script is loaded and a new socket is created
// // Then it is created.
// TEST_F(LuaGenericTest, SocketCreation) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/socket.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  local socket = broker_tcp_socket.new()\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n\n");
//   ASSERT_NO_THROW(new luabinding(filename, conf, *_cache.get()));
//   RemoveFile(filename);
// }
// 
// // When a script is loaded, a new socket is created
// // And a call to connect is made without argument
// // Then it fails.
// TEST_F(LuaGenericTest, SocketConnectionWithoutArg) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/socket.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  local socket = broker_tcp_socket.new()\n"
//                          "  socket:connect()\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n\n");
//   ASSERT_THROW(new luabinding(filename, conf, *_cache.get()), std::exception);
//   RemoveFile(filename);
// }
// 
// // When a script is loaded, a new socket is created
// // And a call to connect is made without argument
// // Then it fails.
// TEST_F(LuaGenericTest, SocketConnectionWithNoPort) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/socket.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  local socket = broker_tcp_socket.new()\n"
//                          "  socket:connect('127.0.0.1')\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n\n");
//   ASSERT_THROW(new luabinding(filename, conf, *_cache.get()), std::exception);
//   RemoveFile(filename);
// }
// 
// #if 0
// // Thoses tests need a little server working on 127.0.0.1:9200
// 
// // When a script is loaded, a new socket is created
// // And a call to connect is made with a good adress/port
// // Then it succeeds.
// TEST_F(LuaGenericTest, SocketConnectionOk) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/socket.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  local socket = broker_tcp_socket.new()\n"
//                          "  socket:connect('127.0.0.1', 9200)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n\n");
//   ASSERT_NO_THROW(new luabinding(filename, conf, *_cache.get()));
//   RemoveFile(filename);
// }
// 
// // When a script is loaded, a new socket is created
// // And a call to get_state is made
// // Then it succeeds, and the return value is Unconnected.
// TEST_F(LuaGenericTest, SocketUnconnectedState) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/socket.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local socket = broker_tcp_socket.new()\n"
//                          "  local state = socket:get_state()\n"
//                          "  broker_log:info(1, 'State: ' .. state)\n"
//                          "  socket:connect('127.0.0.1', 9200)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n\n");
//   luabinding *binding (new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*State: unconnected")) != -1);
//   delete binding;
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a script is loaded, a new socket is created
// // And a call to get_state is made
// // Then it succeeds, and the return value is Unconnected.
// TEST_F(LuaGenericTest, SocketConnectedState) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/socket.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local socket = broker_tcp_socket.new()\n"
//                          "  socket:connect('127.0.0.1', 9200)\n"
//                          "  local state = socket:get_state()\n"
//                          "  broker_log:info(1, 'State: ' .. state)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n\n");
//   luabinding *binding (new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*State: connected")) != -1);
//   delete binding;
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a script is loaded, a new socket is created
// // And a call to connect is made with a good adress/port
// // Then it succeeds.
// TEST_F(LuaGenericTest, SocketWrite) {
//   QMap<QString, QVariant> conf;
//   std::string filename(FILE4);
//   ASSERT_NO_THROW(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
//   ASSERT_TRUE(lst.size() > 0);
//   //RemoveFile("/tmp/log");
// }
// #endif
// 
// // When a script is loaded, a new socket is created
// // And a call to connect is made with a good adress/port
// // Then it succeeds.
// TEST_F(LuaGenericTest, JsonEncode) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/json_encode.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  broker_log:info('coucou')\n"
//                          "  local a = { aa='bonjour',bb=12,cc={'a', 'b', 'c', 4},dd=true}\n"
//                          "  local json = broker.json_encode(a)\n"
//                          "  local b = broker.json_decode(json)\n"
//                          "  for i,v in pairs(b) do\n"
//                          "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
//                          "  end"
//                          "  broker_log:info(1, json)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: aa=>bonjour")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: bb=>12")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: cc=>table: .*")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: dd=>true")) != -1);
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // Given an empty array,
// // Then json_encode() works well on it.
// TEST_F(LuaGenericTest, EmptyJsonEncode) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/json_encode.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local a = {}\n"
//                          "  local json = broker.json_encode(a)\n"
//                          "  broker_log:info(1, 'empty array: ' .. json)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("INFO: empty array: []"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a script is loaded, a new socket is created
// // And a call to connect is made with a good adress/port
// // Then it succeeds.
// TEST_F(LuaGenericTest, JsonEncodeEscape) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/json_encode.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local a = { 'bonjour le \"monde\"', 12, true, 27.1, {a=1, b=2, c=3, d=4}, 'une tabulation\\t...'}\n"
//                          "  local json = broker.json_encode(a)\n"
//                          "  local b = broker.json_decode(json)\n"
//                          "  for i,v in ipairs(b) do\n"
//                          "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
//                          "  end"
//                          "  broker_log:info(1, json)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 1=>bonjour le \"monde\"")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 2=>12")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 3=>true")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 4=>27.1")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 5=>table: .*")) != -1);
//   ASSERT_TRUE(lst.indexOf(QRegExp(".*INFO: 6=>une tabulation\t...")) != -1);
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a hostname is made
// // And the cache does not know about it
// // Then nil is returned from the lua method.
// TEST_F(LuaGenericTest, CacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hst = broker_cache:get_hostname(1)\n"
//                          "  if not hst then\n"
//                          "    broker_log:info(1, 'host does not exist')\n"
//                          "  end\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("host does not exist"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a hostname is made
// // And the cache knows about it
// // Then the hostname is returned from the lua method.
// TEST_F(LuaGenericTest, HostCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::host> hst(new neb::host);
//   hst->host_id = 1;
//   hst->host_name = strdup("centreon");
//   _cache->write(hst);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hst = broker_cache:get_hostname(1)\n"
//                          "  broker_log:info(1, 'host is ' .. tostring(hst))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("host is centreon"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a hostname is made
// // And the cache knows about it
// // Then the hostname is returned from the lua method.
// TEST_F(LuaGenericTest, ServiceCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::service> svc(new neb::service);
//   svc->host_id = 1;
//   svc->service_id = 14;
//   svc->service_description = strdup("description");
//   _cache->write(svc);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local svc = broker_cache:get_service_description(1, 14)\n"
//                          "  broker_log:info(1, 'service description is ' .. tostring(svc))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("service description is description"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a hostname is made
// // And the cache knows about it
// // Then the hostname is returned from the lua method.
// TEST_F(LuaGenericTest, IndexMetricCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::service> svc(new neb::service);
//   svc->host_id = 1;
//   svc->service_id = 14;
//   svc->service_description = strdup("MyDescription");
//   _cache->write(svc);
//   shared_ptr<neb::host> hst(new neb::host);
//   hst->host_id = 1;
//   hst->host_name = strdup("host1");
//   _cache->write(hst);
//   shared_ptr<storage::index_mapping> im(new storage::index_mapping);
//   im->index_id = 7;
//   im->service_id = 14;
//   im->host_id = 1;
//   _cache->write(im);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local index_mapping = broker_cache:get_index_mapping(7)\n"
//                          "  local svc = broker_cache:get_service_description(index_mapping.host_id, index_mapping.service_id)\n"
//                          "  broker_log:info(1, 'service description is ' .. tostring(svc))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("service description is MyDescription"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for an instance is made
// // And the cache knows about it
// // Then the instance is returned from the lua method.
// TEST_F(LuaGenericTest, InstanceNameCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<instance_broadcast> ib(new instance_broadcast);
//   ib->broker_id = 42;
//   ib->broker_name = "broker name";
//   ib->enabled = true;
//   ib->poller_id = 18;
//   ib->poller_name = "MyPoller";
//   _cache->write(ib);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local instance_name = broker_cache:get_instance_name(18)\n"
//                          "  broker_log:info(1, 'instance name is ' .. tostring(instance_name))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("instance name is MyPoller"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a metric mapping is made
// // And the cache knows about it
// // Then the metric mapping is returned from the lua method.
// TEST_F(LuaGenericTest, MetricMappingCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<storage::metric_mapping> mm(new storage::metric_mapping);
//   mm->index_id = 19;
//   mm->metric_id = 27;
//   _cache->write(mm);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local mm = broker_cache:get_metric_mapping(27)\n"
//                          "  broker_log:info(1, 'metric id is ' .. mm.metric_id)\n"
//                          "  broker_log:info(1, 'index id is ' .. mm.index_id)\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("metric id is 27"));
//   ASSERT_TRUE(lst[1].contains("index id is 19"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a host group name is made
// // And the cache does not know about it
// // Then nil is returned by the lua method.
// TEST_F(LuaGenericTest, HostGroupCacheTestNameNotAvailable) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hg = broker_cache:get_hostgroup_name(28)\n"
//                          "  broker_log:info(1, 'host group is ' .. tostring(hg))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("host group is nil"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a host group name is made
// // And the cache does know about it
// // Then the name is returned by the lua method.
// TEST_F(LuaGenericTest, HostGroupCacheTestName) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::host_group> hg(new neb::host_group);
//   hg->id = 28;
//   hg->name = strdup("centreon");
//   _cache->write(hg);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hg = broker_cache:get_hostgroup_name(28)\n"
//                          "  broker_log:info(1, 'host group is ' .. tostring(hg))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("host group is centreon"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for host groups is made
// // And the host is attached to no group
// // Then an empty array is returned.
// TEST_F(LuaGenericTest, HostGroupCacheTestEmpty) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hg = broker_cache:get_hostgroups(1)\n"
//                          "  broker_log:info(1, 'host group is ' .. broker.json_encode(hg))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("host group is []"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for host groups is made
// // And the cache does know about them
// // Then an array is returned by the lua method.
// TEST_F(LuaGenericTest, HostGroupCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::host_group> hg(new neb::host_group);
//   hg->id = 16;
//   hg->name = strdup("centreon1");
//   _cache->write(hg);
//   hg = new neb::host_group;
//   hg->id = 17;
//   hg->name = strdup("centreon2");
//   _cache->write(hg);
//   shared_ptr<neb::host> hst(new neb::host);
//   hst->host_id = 22;
//   hst->host_name = strdup("host_centreon");
//   _cache->write(hst);
//   shared_ptr<neb::host_group_member> member(new neb::host_group_member);
//   member->host_id = 22;
//   member->group_id = 16;
//   member->group_name = "sixteen";
//   member->enabled = false;
//   member->poller_id = 14;
//   _cache->write(member);
//   member = new neb::host_group_member;
//   member->host_id = 22;
//   member->group_id = 17;
//   member->group_name = "seventeen";
//   member->enabled = true;
//   member->poller_id = 144;
//   _cache->write(member);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hg = broker_cache:get_hostgroups(22)\n"
//                          "  for i,v in ipairs(hg) do\n"
//                          "    broker_log:info(1, 'member of ' .. broker.json_encode(v))\n"
//                          "  end\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("\"group_id\":17"));
//   ASSERT_TRUE(lst[0].contains("\"group_name\":\"seventeen\""));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a service group name is made
// // And the cache does not know about it
// // Then nil is returned by the lua method.
// TEST_F(LuaGenericTest, ServiceGroupCacheTestNameNotAvailable) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local hg = broker_cache:get_servicegroup_name(28)\n"
//                          "  broker_log:info(1, 'service group is ' .. tostring(hg))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("service group is nil"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for a service group name is made
// // And the cache does know about it
// // Then the name is returned by the lua method.
// TEST_F(LuaGenericTest, ServiceGroupCacheTestName) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::service_group> sg(new neb::service_group);
//   sg->id = 28;
//   sg->name = strdup("centreon");
//   _cache->write(sg);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local sg = broker_cache:get_servicegroup_name(28)\n"
//                          "  broker_log:info(1, 'service group is ' .. tostring(sg))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("service group is centreon"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for service groups is made
// // And the service is attached to no group
// // Then an empty array is returned.
// TEST_F(LuaGenericTest, ServiceGroupCacheTestEmpty) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local sg = broker_cache:get_servicegroups(1, 3)\n"
//                          "  broker_log:info(1, 'service group is ' .. broker.json_encode(sg))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("service group is []"));
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for service groups is made
// // And the cache does know about them
// // Then an array is returned by the lua method.
// TEST_F(LuaGenericTest, ServiceGroupCacheTest) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::service_group> sg(new neb::service_group);
//   sg->id = 16;
//   sg->name = strdup("centreon1");
//   _cache->write(sg);
//   sg = new neb::service_group;
//   sg->id = 17;
//   sg->name = strdup("centreon2");
//   _cache->write(sg);
//   shared_ptr<neb::service> svc(new neb::service);
//   svc->service_id = 17;
//   svc->host_id = 22;
//   svc->host_name = strdup("host_centreon");
//   svc->service_description = strdup("service_description");
//   _cache->write(svc);
//   shared_ptr<neb::service_group_member> member(new neb::service_group_member);
//   member->host_id = 22;
//   member->service_id = 17;
//   member->poller_id = 3;
//   member->enabled = false;
//   member->group_id = 16;
//   member->group_name = "seize";
//   _cache->write(member);
//   member = new neb::service_group_member;
//   member->host_id = 22;
//   member->service_id = 17;
//   member->poller_id = 4;
//   member->enabled = true;
//   member->group_id = 17;
//   member->group_name = "dix-sept";
//   _cache->write(member);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local sg = broker_cache:get_servicegroups(22, 17)\n"
//                          "  for i,v in ipairs(sg) do\n"
//                          "    broker_log:info(1, 'member of ' .. broker.json_encode(v))\n"
//                          "  end\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("\"group_id\":17"));
//   ASSERT_TRUE(lst[0].contains("\"group_name\":\"dix-sept\""));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for service groups is made
// // And the cache does know about them
// // Then an array is returned by the lua method.
// TEST_F(LuaGenericTest, SetNewInstance) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<neb::service_group> sg(new neb::service_group);
//   sg->id = 16;
//   sg->name = strdup("centreon1");
//   _cache->write(sg);
//   sg = new neb::service_group;
//   sg->id = 17;
//   sg->name = strdup("centreon2");
//   _cache->write(sg);
//   shared_ptr<neb::host> hst(new neb::host);
//   hst->host_id = 22;
//   hst->host_name = strdup("host_centreon");
//   hst->poller_id = 3;
//   _cache->write(hst);
//   shared_ptr<neb::host_group> hg(new neb::host_group);
//   hg->id = 19;
//   hg->name = strdup("hg1");
//   _cache->write(hg);
//   shared_ptr<neb::service> svc(new neb::service);
//   svc->service_id = 17;
//   svc->host_id = 22;
//   svc->host_name = strdup("host_centreon");
//   svc->service_description = strdup("service_description");
//   _cache->write(svc);
//   shared_ptr<neb::host_group_member> hmember(new neb::host_group_member);
//   hmember->host_id = 22;
//   hmember->poller_id = 3;
//   hmember->enabled = true;
//   hmember->group_id = 19;
//   hmember->group_name = "hg1";
//   _cache->write(hmember);
//   shared_ptr<neb::service_group_member> member(new neb::service_group_member);
//   member->host_id = 22;
//   member->service_id = 17;
//   member->poller_id = 3;
//   member->enabled = false;
//   member->group_id = 16;
//   member->group_name = "seize";
//   _cache->write(member);
//   member = new neb::service_group_member;
//   member->host_id = 22;
//   member->service_id = 17;
//   member->poller_id = 3;
//   member->enabled = true;
//   member->group_id = 17;
//   member->group_name = "dix-sept";
//   _cache->write(member);
// 
//   shared_ptr<instance_broadcast> ib(new instance_broadcast);
//   ib->broker_id = 42;
//   ib->broker_name = "broker name";
//   ib->enabled = true;
//   ib->poller_id = 3;
//   ib->poller_name = "MyPoller";
//   _cache->write(ib);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local s = broker_cache:get_service_description(22, 17)\n"
//                          "  broker_log:info(1, 'service description ' .. tostring(s))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("service description nil"));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // When a query for bvs containing a ba is made
// // And the cache does know about them
// // Then an array with bvs id is returned by the lua method.
// TEST_F(LuaGenericTest, BamCacheTestBvBaRelation) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<bam::dimension_ba_bv_relation_event> rel(
//     new bam::dimension_ba_bv_relation_event);
//   rel->ba_id = 10;
//   rel->bv_id = 18;
//   _cache->write(rel);
// 
//   rel = new bam::dimension_ba_bv_relation_event;
//   rel->ba_id = 10;
//   rel->bv_id = 23;
//   _cache->write(rel);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local rel = broker_cache:get_bvs(10)\n"
//                          "  for i,v in ipairs(rel) do\n"
//                          "    broker_log:info(1, 'member of bv ' .. v)\n"
//                          "  end\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   int first, second;
//   if (lst[0].contains("member of bv 18")) {
//     first = 0;
//     second = 1;
//   }
//   else {
//     first = 1;
//     second = 0;
//   }
//   ASSERT_TRUE(lst[first].contains("member of bv 18"));
//   ASSERT_TRUE(lst[second].contains("member of bv 23"));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // Given a ba id,
// // When the Lua get_ba() function is called with it,
// // Then a table corresponding to this ba is returned.
// TEST_F(LuaGenericTest, BamCacheTestBa) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<bam::dimension_ba_event> ba(
//     new bam::dimension_ba_event);
//   ba->ba_id = 10;
//   ba->ba_name = "ba name";
//   ba->ba_description = "ba description";
//   ba->sla_month_percent_crit = 1.25;
//   ba->sla_month_percent_warn = 1.18;
//   ba->sla_duration_crit = 19;
//   ba->sla_duration_warn = 23;
//   _cache->write(ba);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local ba = broker_cache:get_ba(10)\n"
//                          "  broker_log:info(1, 'member of ba ' .. broker.json_encode(ba))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("\"ba_name\":\"ba name\""));
//   ASSERT_TRUE(lst[0].contains("\"ba_description\":\"ba description\""));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // Given a ba id,
// // When the Lua get_ba() function is called with it,
// // And the cache does not know about it,
// // Then nil is returned.
// TEST_F(LuaGenericTest, BamCacheTestBaNil) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local ba = broker_cache:get_ba(10)\n"
//                          "  broker_log:info(1, 'member of ba ' .. tostring(ba))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("member of ba nil"));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // Given a bv id,
// // When the Lua get_bv() function is called with it,
// // Then a table corresponding to this bv is returned.
// TEST_F(LuaGenericTest, BamCacheTestBv) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
//   shared_ptr<bam::dimension_bv_event> bv(
//     new bam::dimension_bv_event);
//   bv->bv_id = 10;
//   bv->bv_name = "bv name";
//   bv->bv_description = "bv description";
//   _cache->write(bv);
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local bv = broker_cache:get_bv(10)\n"
//                          "  broker_log:info(1, 'member of bv ' .. broker.json_encode(bv))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("\"bv_id\":10"));
//   ASSERT_TRUE(lst[0].contains("\"bv_name\":\"bv name\""));
//   ASSERT_TRUE(lst[0].contains("\"bv_description\":\"bv description\""));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
// 
// // Given a bv id,
// // When the Lua get_bv() function is called with it,
// // And the cache does not know about it,
// // Then nil is returned.
// TEST_F(LuaGenericTest, BamCacheTestBvNil) {
//   QMap<QString, QVariant> conf;
//   std::string filename("/tmp/cache_test.lua");
// 
//   CreateScript(filename, "function init(conf)\n"
//                          "  broker_log:set_parameters(3, '/tmp/log')\n"
//                          "  local bv = broker_cache:get_bv(10)\n"
//                          "  broker_log:info(1, 'member of bv ' .. tostring(bv))\n"
//                          "end\n\n"
//                          "function write(d)\n"
//                          "end\n");
//   std::auto_ptr<luabinding> binding(new luabinding(filename, conf, *_cache.get()));
//   QStringList lst(ReadFile("/tmp/log"));
// 
//   ASSERT_TRUE(lst[0].contains("member of bv nil"));
// 
//   RemoveFile(filename);
//   RemoveFile("/tmp/log");
// }
