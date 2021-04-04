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

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <list>
#include <memory>

#include "../../core/test/test_server.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/lua/luabinding.hh"
#include "com/centreon/broker/lua/macro_cache.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/misc/variant.hh"
#include "com/centreon/broker/modules/handle.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::lua;

#define FILE1 CENTREON_BROKER_LUA_SCRIPT_PATH "/test1.lua"
#define FILE2 CENTREON_BROKER_LUA_SCRIPT_PATH "/test2.lua"
#define FILE3 CENTREON_BROKER_LUA_SCRIPT_PATH "/test3.lua"
#define FILE4 CENTREON_BROKER_LUA_SCRIPT_PATH "/socket.lua"

class LuaTest : public ::testing::Test {
 public:
  void SetUp() override {
    try {
      config::applier::init(0, "test_broker");
    } catch (std::exception const& e) {
      (void)e;
    }
    std::shared_ptr<persistent_cache> pcache(
        std::make_shared<persistent_cache>("/tmp/broker_test_cache"));
    _cache.reset(new macro_cache(pcache));
  }
  void TearDown() override {
    // The cache must be destroyed before the applier deinit() call.
    _cache.reset();
    config::applier::deinit();
    ::remove("/tmp/broker_test_cache");
  }

  void CreateScript(std::string const& filename, std::string const& content) {
    std::ofstream oss(filename);
    oss << content;
  }

  std::string ReadFile(std::string const& filename) {
    std::ostringstream oss;
    std::string retval;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line))
      oss << line << '\n';
    return oss.str();
  }

  void RemoveFile(std::string const& filename) {
    std::remove(filename.c_str());
  }

 protected:
  std::unique_ptr<macro_cache> _cache;
};

class LuaAsioTest : public LuaTest {
 public:
  void SetUp() override {
    LuaTest::SetUp();
    _server.init();
    _thread = std::thread(&test_server::run, &_server);

    _server.wait_for_init();
  }
  void TearDown() override {
    LuaTest::TearDown();
    _server.stop();
    _thread.join();
  }

  test_server _server;
  std::thread _thread;
};

// When a lua script that does not exist is loaded
// Then an exception is thrown
TEST_F(LuaTest, MissingScript) {
  std::map<std::string, misc::variant> conf;
  ASSERT_THROW(new luabinding(FILE1, conf, *_cache), msg_fmt);
}

// When a lua script with error such as number divided by nil is loaded
// Then an exception is thrown
TEST_F(LuaTest, FaultyScript) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/faulty.lua");
  CreateScript(filename,
               "local a = { 1, 2, 3 }\n"
               "local b = 18 / a[4]");
  ASSERT_THROW(new luabinding(filename, conf, *_cache), msg_fmt);
  RemoveFile(filename);
}

// When a lua script that does not contain an init() function is loaded
// Then an exception is thrown
TEST_F(LuaTest, WithoutInit) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/without_init.lua");
  CreateScript(filename, "local a = { 1, 2, 3 }\n");
  ASSERT_THROW(new luabinding(filename, conf, *_cache), msg_fmt);
  RemoveFile(filename);
}

// When a lua script that does not contain a filter() function is loaded
// Then has_filter() method returns false
TEST_F(LuaTest, WithoutFilter) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/without_filter.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "function write(d)\n"
               "  return 1\n"
               "end");
  std::unique_ptr<luabinding> bb(new luabinding(filename, conf, *_cache));
  ASSERT_FALSE(bb->has_filter());
  RemoveFile(filename);
}

// When a json parameters file exists but the lua script is incomplete
// Then an exception is thrown
TEST_F(LuaTest, IncompleteScript) {
  std::map<std::string, misc::variant> conf;
  ASSERT_THROW(new luabinding(FILE2, conf, *_cache), msg_fmt);
}

// When a script is correctly loaded and a neb event has to be sent
// Then this event is translated into a Lua table and sent to the lua write()
// function.
TEST_F(LuaTest, SimpleScript) {
  RemoveFile("/tmp/test.log");
  std::map<std::string, misc::variant> conf;
  conf.insert({"address", "127.0.0.1"});
  conf.insert({"port", 8857});
  modules::handle h("./neb/10-neb.so");

  std::string filename("/tmp/test-lua3.lua");
  CreateScript(
      filename,
      "broker_api_version = 2\n"
      "function init(params)\n"
      "  broker_log:set_parameters(1, \"/tmp/test.log\")\n"
      "  for i,v in pairs(params) do\n"
      "    broker_log:info(1, \"init: \" .. i .. \" => \" .. tostring(v))\n"
      "  end\n"
      "end\n"
      "function write(d)\n"
      "  for i,v in pairs(d) do\n"
      "    broker_log:info(1, \"write: \" .. i .. \" => \" .. tostring(v))\n"
      "  end\n"
      "  return true\n"
      "end\n"
      "function filter(typ, cat)\n"
      "  return true\n"
      "end\n");

  std::unique_ptr<luabinding> bnd(new luabinding(filename, conf, *_cache));
  ASSERT_TRUE(bnd.get());
  std::unique_ptr<neb::service> s(new neb::service);
  s->host_id = 12;
  s->service_id = 18;
  s->output = "Bonjour";
  std::shared_ptr<io::data> svc(s.release());
  bnd->write(svc);

  std::string result(ReadFile("/tmp/test.log"));
  std::list<std::string> lst{misc::string::split(result, '\n')};
  // 85 lines and one empty line.
  ASSERT_EQ(lst.size(), 86u);
  size_t pos1 = result.find("INFO: init: address => 127.0.0.1");
  size_t pos2 = result.find("INFO: init: port => 8857");
  size_t pos3 = result.find("INFO: write: host_id => 12");
  size_t pos4 = result.find("INFO: write: output => Bonjour");
  size_t pos5 = result.find("INFO: write: service_id => 18");

  ASSERT_NE(pos1, std::string::npos);
  ASSERT_NE(pos2, std::string::npos);
  ASSERT_NE(pos3, std::string::npos);
  ASSERT_NE(pos4, std::string::npos);
  ASSERT_NE(pos5, std::string::npos);
  RemoveFile(filename);
}

// When a script is correctly loaded and a neb event has to be sent
// Then this event is translated into a Lua table and sent to the lua write()
// function.
TEST_F(LuaTest, WriteAcknowledgement) {
  RemoveFile("/tmp/test.log");
  std::map<std::string, misc::variant> conf;
  conf.insert({"address", "127.0.0.1"});
  conf.insert({"double", 3.14159265358979323846});
  conf.insert({"port", 8857});
  conf.insert({"name", "test-centreon"});
  modules::handle h("./neb/10-neb.so");

  std::unique_ptr<luabinding> bnd(new luabinding(FILE3, conf, *_cache));
  ASSERT_TRUE(bnd.get());
  std::unique_ptr<neb::acknowledgement> s(new neb::acknowledgement);
  s->host_id = 13;
  s->author = "testAck";
  s->service_id = 21;
  std::shared_ptr<io::data> svc(s.release());
  bnd->write(svc);

  std::string result{ReadFile("/tmp/test.log")};
  {
    std::list<std::string> lst{misc::string::split(result, '\n')};
    // 20 = 19 lines + 1 empty line
    std::cout << result << std::endl;
    ASSERT_EQ(lst.size(), 20u);
  }
  ASSERT_NE(result.find("INFO: init: address => 127.0.0.1"), std::string::npos);
  ASSERT_NE(result.find("INFO: init: double => 3.1415926535898"),
            std::string::npos);
  ASSERT_NE(result.find("INFO: init: port => 8857"), std::string::npos);
  ASSERT_NE(result.find("INFO: init: name => test-centreon"),
            std::string::npos);
  ASSERT_NE(result.find("INFO: write: host_id => 13"), std::string::npos);
  ASSERT_NE(result.find("INFO: write: author => testAck"), std::string::npos);
  ASSERT_NE(result.find("INFO: write: service_id => 21"), std::string::npos);
}

// When a script is loaded and a new socket is created
// Then it is created.
TEST_F(LuaTest, SocketCreation) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  local socket = broker_tcp_socket.new()\n"
               "end\n\n"
               "function write(d)\n"
               "end\n\n");
  luabinding* bind = nullptr;
  ASSERT_NO_THROW(bind = new luabinding(filename, conf, *_cache));
  delete bind;
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to connect is made without argument
// Then it fails.
TEST_F(LuaTest, SocketConnectionWithoutArg) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  local socket = broker_tcp_socket.new()\n"
               "  socket:connect()\n"
               "end\n\n"
               "function write(d)\n"
               "end\n\n");
  ASSERT_THROW(new luabinding(filename, conf, *_cache), std::exception);
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to connect is made without argument
// Then it fails.
TEST_F(LuaTest, SocketConnectionWithNoPort) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/socket.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  local socket = broker_tcp_socket.new()\n"
               "  socket:connect('127.0.0.1')\n"
               "end\n\n"
               "function write(d)\n"
               "end\n\n");
  ASSERT_THROW(new luabinding(filename, conf, *_cache), std::exception);
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaAsioTest, SocketConnectionOk) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/socket.lua");

  ASSERT_TRUE(_server.get_bind_ok());

  CreateScript(filename,
               "function init(conf)\n"
               "  local socket = broker_tcp_socket.new()\n"
               "  socket:connect('127.0.0.1', 4242)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n\n");
  std::unique_ptr<luabinding> binding;
  ASSERT_NO_THROW(binding.reset(new luabinding(filename, conf, *_cache)));
  RemoveFile(filename);
}

// When a script is loaded, a new socket is created
// And a call to get_state is made
// Then it succeeds, and the return value is Unconnected.
TEST_F(LuaAsioTest, SocketUnconnectedState) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/socket.lua");

  ASSERT_TRUE(_server.get_bind_ok());

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local socket = broker_tcp_socket.new()\n"
               "  local state = socket:get_state()\n"
               "  broker_log:info(1, 'State: ' .. state)\n"
               "  socket:connect('127.0.0.1', 4242)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("State: unconnected"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a script is loaded, a new socket is created
// And a call to get_state is made
// Then it succeeds, and the return value is Unconnected.
TEST_F(LuaAsioTest, SocketConnectedState) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/socket.lua");

  ASSERT_TRUE(_server.get_bind_ok());

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local socket = broker_tcp_socket.new()\n"
               "  socket:connect('127.0.0.1', 4242)\n"
               "  local state = socket:get_state()\n"
               "  broker_log:info(1, 'State: ' .. state)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("State: connected"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaAsioTest, SocketWrite) {
  std::map<std::string, misc::variant> conf;
  std::string filename(FILE4);

  ASSERT_TRUE(_server.get_bind_ok());

  std::unique_ptr<luabinding> binding;
  ASSERT_NO_THROW(binding.reset(new luabinding(filename, conf, *_cache)));
  std::string lst(ReadFile("/tmp/log"));
  ASSERT_TRUE(lst.size() > 0);
  RemoveFile("/tmp/log");
}

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaTest, JsonEncode) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  broker_log:info('coucou')\n"
      "  local a = { aa='C:\\\\bonjour',bb=12,cc={'a', 'b', 'c', 4},dd=true}\n"
      "  local json = broker.json_encode(a)\n"
      "  local b = broker.json_decode(json)\n"
      "  for i,v in pairs(b) do\n"
      "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
      "  end"
      "  broker_log:info(1, json)\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));

  ASSERT_NE(result.find("INFO: aa=>C:\\bonjour"), std::string::npos);
  ASSERT_NE(result.find("INFO: bb=>12"), std::string::npos);
  ASSERT_NE(result.find("INFO: cc=>table: "), std::string::npos);
  ASSERT_NE(result.find("INFO: dd=>true"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given an empty array,
// Then json_encode() works well on it.
TEST_F(LuaTest, EmptyJsonEncode) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local a = {}\n"
               "  local json = broker.json_encode(a)\n"
               "  broker_log:info(1, 'empty array: ' .. json)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("INFO: empty array: []"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a script is loaded, a new socket is created
// And a call to connect is made with a good adress/port
// Then it succeeds.
TEST_F(LuaTest, JsonEncodeEscape) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local a = { 'd:\\\\bonjour le \"monde\"', 12, true, 27.1, "
               "{a=1, b=2, c=3, d=4}, 'une tabulation\\t...'}\n"
               "  local json = broker.json_encode(a)\n"
               "  local b = broker.json_decode(json)\n"
               "  for i,v in ipairs(b) do\n"
               "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
               "  end"
               "  broker_log:info(1, json)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("INFO: 1=>d:\\bonjour le \"monde\""), std::string::npos);
  ASSERT_NE(lst.find("INFO: 2=>12"), std::string::npos);
  ASSERT_NE(lst.find("INFO: 3=>true"), std::string::npos);
  ASSERT_NE(lst.find("INFO: 4=>27.1"), std::string::npos);
  ASSERT_NE(lst.find("INFO: 5=>table: "), std::string::npos);
  ASSERT_NE(lst.find("INFO: 6=>une tabulation\t..."), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a script is loaded with a lua table containing three keys
// "category" with 1, "element": with 4 and "type" that is the concatenation
// of the two previous ones.
// And a call to json_encode is made on that table
// Then it succeeds.
TEST_F(LuaTest, JsonEncodeEvent) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_encode.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local a = { category = 1, element = 4, type = 65540 }\n"
               "  local json = broker.json_encode(a)\n"
               "  local b = broker.json_decode(json)\n"
               "  for i,v in pairs(b) do\n"
               "    broker_log:info(1, i .. '=>' .. tostring(v))\n"
               "  end\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result{ReadFile("/tmp/log")};

  ASSERT_NE(result.find("INFO: category=>1"), std::string::npos);
  ASSERT_NE(result.find("INFO: element=>4"), std::string::npos);
  ASSERT_NE(result.find("INFO: type=>65540"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache does not know about it
// Then nil is returned from the lua method.
TEST_F(LuaTest, CacheTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local hst = broker_cache:get_hostname(1)\n"
               "  if not hst then\n"
               "    broker_log:info(1, 'host does not exist')\n"
               "  end\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("host does not exist"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache knows about it
// Then the hostname is returned from the lua method.
TEST_F(LuaTest, HostCacheTest) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->host_name = "centreon";
  hst->alias = "alias-centreon";
  hst->address = "4.3.2.1";
  _cache->write(hst);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local hstname = broker_cache:get_hostname(1)\n"
               "  broker_log:info(1, 'host is ' .. tostring(hstname))\n"
               "  local hst = broker_cache:get_host(1)\n"
               "  broker_log:info(1, 'alias ' .. hst.alias .. ' address ' .. "
               "hst.address .. ' name ' .. hst.name)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("host is centreon"), std::string::npos);
  ASSERT_NE(lst.find("alias alias-centreon address 4.3.2.1 name centreon"),
            std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache knows about it
// Then the hostname is returned from the lua method.
TEST_F(LuaTest, ServiceCacheTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 14;
  svc->service_description = "description";
  _cache->write(svc);

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local svc = broker_cache:get_service_description(1, 14)\n"
      "  broker_log:info(1, 'service description is ' .. tostring(svc))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("service description is description"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache knows about it
// Then the hostname is returned from the lua method.
TEST_F(LuaTest, IndexMetricCacheTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 14;
  svc->service_description = "MyDescription";
  _cache->write(svc);
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->host_name = "host1";
  _cache->write(hst);
  std::shared_ptr<storage::index_mapping> im(new storage::index_mapping);
  im->index_id = 7;
  im->service_id = 14;
  im->host_id = 1;
  _cache->write(im);

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local index_mapping = broker_cache:get_index_mapping(7)\n"
      "  local svc = "
      "broker_cache:get_service_description(index_mapping.host_id, "
      "index_mapping.service_id)\n"
      "  broker_log:info(1, 'service description is ' .. tostring(svc))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("service description is MyDescription"),
            std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for an instance is made
// And the cache knows about it
// Then the instance is returned from the lua method.
TEST_F(LuaTest, InstanceNameCacheTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::instance> inst(new neb::instance);
  inst->broker_id = 42;
  inst->engine = "engine name";
  inst->is_running = true;
  inst->poller_id = 18;
  inst->name = "MyPoller";
  _cache->write(inst);

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local instance_name = broker_cache:get_instance_name(18)\n"
      "  broker_log:info(1, 'instance name is ' .. tostring(instance_name))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("instance name is MyPoller"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a metric mapping is made
// And the cache knows about it
// Then the metric mapping is returned from the lua method.
TEST_F(LuaTest, MetricMappingCacheTestV1) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<storage::metric_mapping> mm(new storage::metric_mapping);
  mm->index_id = 19;
  mm->metric_id = 27;
  _cache->write(mm);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local mm = broker_cache:get_metric_mapping(27)\n"
               "  broker_log:info(1, 'mm type is ' .. type(mm))\n"
               "  broker_log:info(1, 'metric id is ' .. mm.metric_id)\n"
               "  broker_log:info(1, 'index id is ' .. mm.index_id)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("mm type is table"));
  ASSERT_NE(std::string::npos, lst.find("metric id is 27"));
  ASSERT_NE(std::string::npos, lst.find("index id is 19"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, MetricMappingCacheTestV2) {
  modules::handle h("./storage/20-storage.so");
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<storage::metric_mapping> mm(new storage::metric_mapping);
  mm->index_id = 19;
  mm->metric_id = 27;
  _cache->write(mm);

  CreateScript(filename,
               "broker_api_version=2\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local mm = broker_cache:get_metric_mapping(27)\n"
               "  broker_log:info(1, 'mm type is ' .. type(mm))\n"
               "  broker_log:info(1, 'metric id is ' .. mm.metric_id)\n"
               "  broker_log:info(1, 'index id is ' .. mm.index_id)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("mm type is userdata"));
  ASSERT_NE(std::string::npos, lst.find("metric id is 27"));
  ASSERT_NE(std::string::npos, lst.find("index id is 19"));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a host group name is made
// And the cache does not know about it
// Then nil is returned by the lua method.
TEST_F(LuaTest, HostGroupCacheTestNameNotAvailable) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local hg = broker_cache:get_hostgroup_name(28)\n"
               "  broker_log:info(1, 'host group is ' .. tostring(hg))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("host group is nil"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a host group name is made
// And the cache does know about it
// Then the name is returned by the lua method.
TEST_F(LuaTest, HostGroupCacheTestName) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::host_group> hg(new neb::host_group);
  hg->id = 28;
  hg->name = "centreon";
  _cache->write(hg);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local hg = broker_cache:get_hostgroup_name(28)\n"
               "  broker_log:info(1, 'host group is ' .. tostring(hg))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("host group is centreon"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for host groups is made
// And the host is attached to no group
// Then an empty array is returned.
TEST_F(LuaTest, HostGroupCacheTestEmpty) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local hg = broker_cache:get_hostgroups(1)\n"
      "  broker_log:info(1, 'host group is ' .. broker.json_encode(hg))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("host group is []"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for host groups is made
// And the cache does know about them
// Then an array is returned by the lua method.
TEST_F(LuaTest, HostGroupCacheTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::host_group> hg(new neb::host_group);
  hg->id = 16;
  hg->name = "centreon1";
  _cache->write(hg);
  hg.reset(new neb::host_group);
  hg->id = 17;
  hg->name = "centreon2";
  _cache->write(hg);
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 22;
  hst->host_name = "host_centreon";
  _cache->write(hst);
  std::shared_ptr<neb::host_group_member> member(new neb::host_group_member);
  member->host_id = 22;
  member->group_id = 16;
  member->group_name = "sixteen";
  member->enabled = false;
  member->poller_id = 14;
  _cache->write(member);
  member.reset(new neb::host_group_member);
  member->host_id = 22;
  member->group_id = 17;
  member->group_name = "seventeen";
  member->enabled = true;
  member->poller_id = 144;
  _cache->write(member);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local hg = broker_cache:get_hostgroups(22)\n"
               "  for i,v in ipairs(hg) do\n"
               "    broker_log:info(1, 'member of ' .. broker.json_encode(v))\n"
               "  end\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("\"group_id\":17"));
  ASSERT_NE(std::string::npos, lst.find("\"group_name\":\"seventeen\""));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a service group name is made
// And the cache does not know about it
// Then nil is returned by the lua method.
TEST_F(LuaTest, ServiceGroupCacheTestNameNotAvailable) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local hg = broker_cache:get_servicegroup_name(28)\n"
               "  broker_log:info(1, 'service group is ' .. tostring(hg))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("service group is nil"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a service group name is made
// And the cache does know about it
// Then the name is returned by the lua method.
TEST_F(LuaTest, ServiceGroupCacheTestName) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service_group> sg(new neb::service_group);
  sg->id = 28;
  sg->name = "centreon";
  _cache->write(sg);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local sg = broker_cache:get_servicegroup_name(28)\n"
               "  broker_log:info(1, 'service group is ' .. tostring(sg))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("service group is centreon"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for service groups is made
// And the service is attached to no group
// Then an empty array is returned.
TEST_F(LuaTest, ServiceGroupCacheTestEmpty) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local sg = broker_cache:get_servicegroups(1, 3)\n"
      "  broker_log:info(1, 'service group is ' .. broker.json_encode(sg))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_TRUE(lst.find("service group is []", std::string::npos));
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for service groups is made
// And the cache does know about them
// Then an array is returned by the lua method.
TEST_F(LuaTest, ServiceGroupCacheTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service_group> sg(new neb::service_group);
  sg->id = 16;
  sg->name = "centreon1";
  _cache->write(sg);
  sg.reset(new neb::service_group);
  sg->id = 17;
  sg->name = "centreon2";
  _cache->write(sg);
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->service_id = 17;
  svc->host_id = 22;
  svc->host_name = "host_centreon";
  svc->service_description = "service_description";
  _cache->write(svc);
  std::shared_ptr<neb::service_group_member> member(
      new neb::service_group_member);
  member->host_id = 22;
  member->service_id = 17;
  member->poller_id = 3;
  member->enabled = false;
  member->group_id = 16;
  member->group_name = "seize";
  _cache->write(member);
  member.reset(new neb::service_group_member);
  member->host_id = 22;
  member->service_id = 17;
  member->poller_id = 4;
  member->enabled = true;
  member->group_id = 17;
  member->group_name = "dix-sept";
  _cache->write(member);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local sg = broker_cache:get_servicegroups(22, 17)\n"
               "  for i,v in ipairs(sg) do\n"
               "    broker_log:info(1, 'member of ' .. broker.json_encode(v))\n"
               "  end\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("\"group_id\":17"));
  ASSERT_NE(std::string::npos, lst.find("\"group_name\":\"dix-sept\""));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for service groups is made
// And the cache does know about them
// Then an array is returned by the lua method.
TEST_F(LuaTest, SetNewInstance) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service_group> sg(new neb::service_group);
  sg->id = 16;
  sg->name = "centreon1";
  _cache->write(sg);
  sg.reset(new neb::service_group);
  sg->id = 17;
  sg->name = "centreon2";
  _cache->write(sg);
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 22;
  hst->host_name = "host_centreon";
  hst->poller_id = 3;
  _cache->write(hst);
  std::shared_ptr<neb::host_group> hg(new neb::host_group);
  hg->id = 19;
  hg->name = "hg1";
  _cache->write(hg);
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->service_id = 17;
  svc->host_id = 22;
  svc->host_name = "host_centreon";
  svc->service_description = "service_description";
  _cache->write(svc);
  std::shared_ptr<neb::host_group_member> hmember(new neb::host_group_member);
  hmember->host_id = 22;
  hmember->poller_id = 3;
  hmember->enabled = true;
  hmember->group_id = 19;
  hmember->group_name = "hg1";
  _cache->write(hmember);
  std::shared_ptr<neb::service_group_member> member(
      new neb::service_group_member);
  member->host_id = 22;
  member->service_id = 17;
  member->poller_id = 3;
  member->enabled = false;
  member->group_id = 16;
  member->group_name = "seize";
  _cache->write(member);
  member.reset(new neb::service_group_member);
  member->host_id = 22;
  member->service_id = 17;
  member->poller_id = 3;
  member->enabled = true;
  member->group_id = 17;
  member->group_name = "dix-sept";
  _cache->write(member);

  std::shared_ptr<neb::instance> ib(new neb::instance);
  ib->broker_id = 42;
  ib->engine = "engine name";
  ib->is_running = true;
  ib->poller_id = 3;
  ib->name = "MyPoller";
  _cache->write(ib);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local s = broker_cache:get_service_description(22, 17)\n"
               "  broker_log:info(1, 'service description ' .. tostring(s))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("service description nil"));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for bvs containing a ba is made
// And the cache does know about them
// Then an array with bvs id is returned by the lua method.
TEST_F(LuaTest, BamCacheTestBvBaRelation) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<bam::dimension_ba_bv_relation_event> rel(
      new bam::dimension_ba_bv_relation_event);
  rel->ba_id = 10;
  rel->bv_id = 18;
  _cache->write(rel);

  rel.reset(new bam::dimension_ba_bv_relation_event);
  rel->ba_id = 10;
  rel->bv_id = 23;
  _cache->write(rel);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local rel = broker_cache:get_bvs(10)\n"
               "  for i,v in ipairs(rel) do\n"
               "    broker_log:info(1, 'member of bv ' .. v)\n"
               "  end\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("member of bv 18"));
  ASSERT_NE(std::string::npos, lst.find("member of bv 23"));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given a ba id,
// When the Lua get_ba() function is called with it,
// Then a table corresponding to this ba is returned.
TEST_F(LuaTest, BamCacheTestBaV1) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<bam::dimension_ba_event> ba(new bam::dimension_ba_event);
  ba->ba_id = 10;
  ba->ba_name = "ba name";
  ba->ba_description = "ba description";
  ba->sla_month_percent_crit = 1.25;
  ba->sla_month_percent_warn = 1.18;
  ba->sla_duration_crit = 19;
  ba->sla_duration_warn = 23;
  _cache->write(ba);

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local ba = broker_cache:get_ba(10)\n"
      "  broker_log:info(1, 'member of ba ' .. broker.json_encode(ba))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("\"ba_name\":\"ba name\""));
  ASSERT_NE(std::string::npos,
            lst.find("\"ba_description\":\"ba description\""));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, BamCacheTestBaV2) {
  modules::handle h("./bam/20-bam.so");
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<bam::dimension_ba_event> ba(new bam::dimension_ba_event);
  ba->ba_id = 10;
  ba->ba_name = "ba name";
  ba->ba_description = "ba description";
  ba->sla_month_percent_crit = 1.25;
  ba->sla_month_percent_warn = 1.18;
  ba->sla_duration_crit = 19;
  ba->sla_duration_warn = 23;
  _cache->write(ba);

  CreateScript(
      filename,
      "broker_api_version=2\n"
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local ba = broker_cache:get_ba(10)\n"
      "  broker_log:info(1, 'member of ba ' .. broker.json_encode(ba))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("\"ba_name\":\"ba name\""));
  ASSERT_NE(std::string::npos,
            lst.find("\"ba_description\":\"ba description\""));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given a ba id,
// When the Lua get_ba() function is called with it,
// And the cache does not know about it,
// Then nil is returned.
TEST_F(LuaTest, BamCacheTestBaNil) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local ba = broker_cache:get_ba(10)\n"
               "  broker_log:info(1, 'member of ba ' .. tostring(ba))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("member of ba nil"));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given a bv id,
// When the Lua get_bv() function is called with it,
// Then a table corresponding to this bv is returned.
TEST_F(LuaTest, BamCacheTestBvV1) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<bam::dimension_bv_event> bv(new bam::dimension_bv_event);
  bv->bv_id = 10;
  bv->bv_name = "bv name";
  bv->bv_description = "bv description";
  _cache->write(bv);

  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local bv = broker_cache:get_bv(10)\n"
      "  broker_log:info(1, 'type of bv ' .. type(bv))\n"
      "  broker_log:info(1, 'member of bv ' .. broker.json_encode(bv))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(std::string::npos, lst.find("type of bv table"));
  ASSERT_NE(std::string::npos, lst.find("\"bv_id\":10"));
  ASSERT_NE(std::string::npos, lst.find("\"bv_name\":\"bv name\""));
  ASSERT_NE(std::string::npos,
            lst.find("\"bv_description\":\"bv description\""));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, BamCacheTestBvV2) {
  modules::handle h("./bam/20-bam.so");
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<bam::dimension_bv_event> bv(new bam::dimension_bv_event);
  bv->bv_id = 10;
  bv->bv_name = "bv name";
  bv->bv_description = "bv description";
  _cache->write(bv);

  CreateScript(
      filename,
      "broker_api_version=2\n"
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local bv = broker_cache:get_bv(10)\n"
      "  broker_log:info(1, 'type of bv ' .. type(bv))\n"
      "  broker_log:info(1, 'member of bv ' .. broker.json_encode(bv))\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  std::cout << lst << std::endl;
  ASSERT_NE(std::string::npos, lst.find("type of bv userdata"));
  ASSERT_NE(std::string::npos, lst.find("\"bv_id\":10"));
  ASSERT_NE(std::string::npos, lst.find("\"bv_name\":\"bv name\""));
  ASSERT_NE(std::string::npos,
            lst.find("\"bv_description\":\"bv description\""));

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

//  // Given a bv id,
//  // When the Lua get_bv() function is called with it,
//  // And the cache does not know about it,
//  // Then nil is returned.
//  TEST_F(LuaTest, BamCacheTestBvNil) {
//    std::map<std::string, misc::variant> conf;
//    std::string filename("/tmp/cache_test.lua");
//
//    CreateScript(filename, "function init(conf)\n"
//                           "  broker_log:set_parameters(3, '/tmp/log')\n"
//                           "  local bv = broker_cache:get_bv(10)\n"
//                           "  broker_log:info(1, 'member of bv ' ..
//                           tostring(bv))\n" "end\n\n" "function write(d)\n"
//                           "end\n");
//    std::unique_ptr<luabinding> binding(new luabinding(filename, conf,
//    *_cache.get())); std::list<std::string> lst(ReadFile("/tmp/log"));
//
//    ASSERT_TRUE(lst[0].contains("member of bv nil"));
//
//    RemoveFile(filename);
//    RemoveFile("/tmp/log");
//  }
//
TEST_F(LuaTest, ParsePerfdata) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/parse_perfdata.lua");

  CreateScript(
      filename,
      "local function test_perf(value, full)\n"
      "  perf, err_msg = broker.parse_perfdata(value, full)\n"
      "  if perf then\n"
      "    broker_log:info(1, broker.json_encode(perf))\n"
      "  else\n"
      "    broker_log:info(1, err_msg)\n"
      "  end \n"
      "end\n\n"
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  test_perf(' percent_packet_loss=0 rta=0.80')\n"
      "  test_perf(\" 'one value'=0 'another value'=0.89\")\n"
      "  test_perf(\" 'one value'=0;3;5 'another value'=0.89;0.8;1;;\")\n"
      "  test_perf(\" 'one value'=1;3;5;0;9 'another "
      "value'=10.89;0.8;1;0;20\")\n"
      "  test_perf(\" 'one value'=2s;3;5;0;9 'a b c'=3.14KB;0.8;1;0;10\")\n"
      "  test_perf(' percent_packet_loss=1.74;50;80;0;100 rta=0.80', true)\n"
      "  test_perf(\" 'one value'=12%;25:80;81:95 'another "
      "value'=78%;60;90;;\", true)\n"
      "  test_perf(\"                \")\n"
      "  test_perf(\" 'one value' 0;3;5\")\n"
      "  test_perf(\" 'events'=;;;0;\")\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));
  size_t pos1 = lst.find("\"percent_packet_loss\":0");
  size_t pos2 = lst.find("\"rta\":0.8");
  size_t pos3 = lst.find("\"one value\":0", pos2 + 1);
  size_t pos4 = lst.find("\"another value\":0.89", pos2 + 1);
  size_t pos5 = lst.find("\"one value\":0", pos4 + 1);
  size_t pos6 = lst.find("\"another value\":0.89", pos4 + 1);
  size_t pos7 = lst.find("\"one value\":1", pos6 + 1);
  size_t pos8 = lst.find("\"another value\":10.89", pos6 + 1);
  size_t pos9 = lst.find("\"one value\":2", pos8 + 1);
  size_t pos10 = lst.find("\"a b c\":3.14", pos8 + 1);
  size_t pos11 = lst.find("\"value\":1.74", pos10 + 1);
  size_t pos12 = lst.find("\"warning_high\":50", pos10 + 1);
  size_t pos13 = lst.find("\"critical_high\":80", pos10 + 1);
  size_t pos14 = lst.find("\"value\":12", pos13 + 1);
  size_t pos15 = lst.find("\"warning_low\":25", pos13 + 1);
  size_t pos16 = lst.find("\"warning_high\":80", pos13 + 1);
  size_t pos17 = lst.find("\"critical_low\":81", pos13 + 1);
  size_t pos18 = lst.find("\"critical_high\":95", pos13 + 1);
  size_t pos19 = lst.find("[]", pos18 + 1);
  size_t pos20 = lst.find("[]", pos19 + 1);

  ASSERT_LE(pos1, pos3);
  ASSERT_LE(pos3, pos5);
  ASSERT_LE(pos5, pos7);
  ASSERT_LE(pos7, pos9);
  ASSERT_LE(pos9, pos11);
  ASSERT_LE(pos12, pos14);
  ASSERT_NE(pos14, std::string::npos);
  ASSERT_NE(pos15, std::string::npos);
  ASSERT_NE(pos16, std::string::npos);
  ASSERT_NE(pos17, std::string::npos);
  ASSERT_NE(pos18, std::string::npos);
  ASSERT_NE(pos19, std::string::npos);
  ASSERT_NE(pos20, std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given a script requiring another one with the same path.
// When the first script call a function defined in the second one
// Then the call works.
TEST_F(LuaTest, UpdatePath) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/test.lua");
  std::string module("/tmp/module.lua");

  CreateScript(module,
               "local my_module = {}\n"
               "function my_module.test()\n"
               "  return 'foo bar'\n"
               "end\n\n"
               "return my_module");
  CreateScript(filename,
               "local my_module = require('module')\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  broker_log:info(0, my_module.test())\n"
               "end\n\n"
               "function write(d)\n"
               "  return true\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("foo bar"), std::string::npos);

  RemoveFile(module);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, CheckPath) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/test.lua");

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  broker_log:info(0, package.path)\n"
               "  broker_log:info(0, package.cpath)\n"
               "end\n\n"
               "function write(d)\n"
               "  return true\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("/tmp/?.lua"), std::string::npos);
  ASSERT_NE(lst.find("/tmp/lib/?.so"), std::string::npos);

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// Given a string
// Then a call to broker.url_encode with this string URL encodes it.
TEST_F(LuaTest, UrlEncode) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/url_encode.lua");
  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local res1 = broker.url_encode('This is a simple line')\n"
      "  if res1 == 'This%20is%20a%20simple%20line' then\n"
      "    broker_log:info(1, 'RES1 GOOD')\n"
      "  end\n"
      "  local res2 = broker.url_encode('La leçon du château de "
      "l\\'araignée')\n"
      "  if res2 == "
      "'La%20le%C3%A7on%20du%20ch%C3%A2teau%20de%20l%27araign%C3%A9e' then\n"
      "    broker_log:info(1, 'RES2 GOOD')\n"
      "  end\n"
      "  local res3 = broker.url_encode('A.a-b_B:c/C?d=D&e~')\n"
      "  if res3 == 'A.a-b_B%3Ac%2FC%3Fd%3DD%26e~' then\n"
      "    broker_log:info(1, 'RES3 GOOD')\n"
      "  end\n"
      "end\n\n"
      "function write(d)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));

  ASSERT_NE(result.find("INFO: RES1 GOOD"), std::string::npos);
  ASSERT_NE(result.find("INFO: RES2 GOOD"), std::string::npos);
  ASSERT_NE(result.find("INFO: RES3 GOOD"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, JsonDecodeArray) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_decode_array.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local test_json = \"[ 2, 3, 5 ]\"\n"
               "  local dec = broker.json_decode(test_json)\n"
               "  broker_log:info(1, \"dec[1]=\" .. tostring(dec[1]))\n"
               "  broker_log:info(1, \"dec[2]=\" .. tostring(dec[2]))\n"
               "  broker_log:info(1, \"dec[3]=\" .. tostring(dec[3]))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));

  ASSERT_NE(result.find("dec[1]=2"), std::string::npos);
  ASSERT_NE(result.find("dec[2]=3"), std::string::npos);
  ASSERT_NE(result.find("dec[3]=5"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, JsonDecodeObject) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_decode_object.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local test_json = '{ \"foo\": 12, \"bar\": \"test\" }'\n"
               "  local dec = broker.json_decode(test_json)\n"
               "  broker_log:info(1, \"dec.foo=\" .. tostring(dec.foo))\n"
               "  broker_log:info(1, \"dec.bar=\" .. tostring(dec.bar))\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));

  ASSERT_NE(result.find("dec.foo=12"), std::string::npos);
  ASSERT_NE(result.find("dec.bar=test"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, JsonDecodeFull) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_decode_full.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local test_json = [[{\n"
               "    \"quiz\": {\n"
               "        \"sport\": {\n"
               "            \"q1\": {\n"
               "                \"question\": \"Which one is correct team name "
               "in NBA?\",\n"
               "                \"options\": [\n"
               "                    \"New York Bulls\",\n"
               "                    \"Los Angeles Kings\",\n"
               "                    \"Golden State Warriros\",\n"
               "                    \"Huston Rocket\"\n"
               "                ],\n"
               "                \"answer\": \"Huston Rocket\"\n"
               "            }\n"
               "        },\n"
               "        \"maths\": {\n"
               "            \"q1\": {\n"
               "                \"question\": \"5 + 7 = ?\",\n"
               "                \"options\": [\n"
               "                    \"10\",\n"
               "                    \"11\",\n"
               "                    \"12\",\n"
               "                    \"13\"\n"
               "                ],\n"
               "                \"answer\": \"12\"\n"
               "            },\n"
               "            \"q2\": {\n"
               "                \"question\": \"12 - 8 = ?\",\n"
               "                \"options\": [\n"
               "                    \"1\",\n"
               "                    \"2\",\n"
               "                    \"3\",\n"
               "                    \"4\"\n"
               "                ],\n"
               "                \"answer\": \"4\"\n"
               "            }\n"
               "        }\n"
               "    }\n"
               "}]]\n"
               "  local dec = broker.json_decode(test_json)\n"
               "  broker_log:info(1, \"dec.quiz.maths.q1.question=\" .. "
               "tostring(dec.quiz.maths.q1.question))\n"
               "  broker_log:info(1, \"dec.quiz.maths.q2.options[2]=\" .. "
               "tostring(dec.quiz.maths.q2.options[2]))\n"
               "end\n\n"
               "function write(d)\n"
               "  return true\n"
               "end");

  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));

  ASSERT_NE(result.find("dec.quiz.maths.q1.question=5 + 7 = ?"),
            std::string::npos);
  ASSERT_NE(result.find("dec.quiz.maths.q2.options[2]=2"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, JsonDecodeError) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/json_decode_error.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local test_json = [[{\n"
               "    \"quiz\": {\n"
               "        \"sport\": {\n"
               "            \"q1\": {\n"
               "                \"question\": \"Which one is correct team name "
               "in NBA?\",\n"
               "                \"options\": [\n"
               "                    \"New York Bulls\",\n"
               "}]]\n"
               "  local dec, err = broker.json_decode(test_json)\n"
               "  broker_log:info(1, \"dec=\" .. tostring(dec))\n"
               "  broker_log:info(1, \"err=\" .. tostring(err))\n"
               "end\n\n"
               "function write(d)\n"
               "  return true\n"
               "end");

  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));

  ASSERT_NE(result.find("dec=nil"), std::string::npos);
  ASSERT_NE(result.find("err=expected value, got '}' (125)"),
            std::string::npos);
  RemoveFile(filename);
  // RemoveFile("/tmp/log");
}

// When the user needs information on a file, he can use the stat function
// that gives several informations about it. On success, the function returns
// an array containing desired information, otherwise, this table is nil but
// a second value is returned: a string with the error message.
TEST_F(LuaTest, Stat) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/stat.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local info = broker.stat('/tmp/stat.lua')\n"
               "  local json = broker.json_encode(info)\n"
               "  broker_log:info(1, json)\n"
               "end\n"
               "function write(d)\n"
               "  return true\n"
               "end");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));
  uid_t uid = geteuid();
  std::string str(fmt::format("\"uid\":{}", uid));
  ASSERT_NE(result.find(str), std::string::npos);
  str = fmt::format("\"uid\":{}", uid);
  ASSERT_NE(result.find(str), std::string::npos);

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, StatError) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/stat.lua");
  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/log')\n"
      "  local info,err = broker.stat('/tmp/statthatdoesnotexist.lua')\n"
      "  broker_log:info(1, \"info=\"..tostring(info))\n"
      "  broker_log:info(1, \"err=\"..tostring(err))\n"
      "end\n"
      "function write(d)\n"
      "  return true\n"
      "end");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));
  ASSERT_NE(result.find("info=nil"), std::string::npos);
  ASSERT_NE(result.find("err=No such file or directory"), std::string::npos);

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache does not know about it
// Then nil is returned from the lua method.
TEST_F(LuaTest, CacheGetNotesUrlTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->notes = "host notes";
  hst->notes_url = "host notes url";
  hst->action_url = "host action url";
  hst->host_name = "centreon";
  _cache->write(hst);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local notes_url = broker_cache:get_notes_url(1)\n"
               "  local action_url = broker_cache:get_action_url(1)\n"
               "  local notes = broker_cache:get_notes(1)\n"
               "  broker_log:info(1, \"notes_url=\" .. notes_url)\n"
               "  broker_log:info(1, \"action_url=\" .. action_url)\n"
               "  broker_log:info(1, \"notes=\" .. notes)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("notes_url=host notes url"), std::string::npos);
  ASSERT_NE(lst.find("action_url=host action url"), std::string::npos);
  ASSERT_NE(lst.find("notes=host notes"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When a query for a hostname is made
// And the cache does not know about it
// Then nil is returned from the lua method.
TEST_F(LuaTest, CacheSvcGetNotesUrlTest) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  _cache->write(svc);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local notes_url = broker_cache:get_notes_url(1, 2)\n"
               "  local action_url = broker_cache:get_action_url(1, 2)\n"
               "  local notes = broker_cache:get_notes(1, 2)\n"
               "  broker_log:info(1, \"notes_url=\" .. notes_url)\n"
               "  broker_log:info(1, \"action_url=\" .. action_url)\n"
               "  broker_log:info(1, \"notes=\" .. notes)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("notes_url=svc notes url"), std::string::npos);
  ASSERT_NE(lst.find("action_url=svc action url"), std::string::npos);
  ASSERT_NE(lst.find("notes=svc notes"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, CacheSeverity) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/cache_test.lua");
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  _cache->write(svc);
  std::shared_ptr<neb::custom_variable> cv =
      std::make_shared<neb::custom_variable>();
  cv->name = "CRITICALITY_LEVEL";
  cv->value = std::to_string(3);
  cv->host_id = 1;
  cv->service_id = 2;
  _cache->write(cv);

  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local severity = broker_cache:get_severity(1, 2)\n"
               "  broker_log:info(1, \"severity=\" .. severity)\n"
               "end\n\n"
               "function write(d)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string lst(ReadFile("/tmp/log"));

  ASSERT_NE(lst.find("severity=3"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

TEST_F(LuaTest, BrokerEventIndex) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->service_description = "foo bar";
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  svc->check_interval = 1.2;
  svc->check_type = 14;
  svc->last_check = 123456;
  std::string filename("/tmp/cache_test.lua");
  CreateScript(
      filename,
      "function init(conf)\n"
      "  broker_log:set_parameters(3, '/tmp/event_log')\n"
      "end\n\n"
      "function write(d)\n"
      "  broker_log:info(0, 'description = ' .. d.description)\n"
      "  broker_log:info(0, 'stalk_on_ok = ' .. tostring(d.stalk_on_ok))\n"
      "  broker_log:info(0, 'check_interval = ' .. d.check_interval)\n"
      "  broker_log:info(0, 'check_type = ' .. d.check_type)\n"
      "  broker_log:info(0, 'service_id = ' .. d.service_id)\n"
      "  broker_log:info(0, 'last_check = ' .. d.last_check)\n"
      "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(svc);
  std::string lst(ReadFile("/tmp/event_log"));
  std::cout << lst << std::endl;
  ASSERT_NE(lst.find("description = foo bar"), std::string::npos);
  ASSERT_NE(lst.find("stalk_on_ok = false"), std::string::npos);
  ASSERT_NE(lst.find("check_interval = 1.2"), std::string::npos);
  ASSERT_NE(lst.find("check_type = 14"), std::string::npos);
  ASSERT_NE(lst.find("service_id = 2"), std::string::npos);
  ASSERT_NE(lst.find("last_check = 123456"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, BrokerEventPairs) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->service_description = "foo bar";
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  for k,v in pairs(d) do\n"
               "    broker_log:info(0, k .. ' = ' .. tostring(v))\n"
               "  end\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(svc);
  std::string lst(ReadFile("/tmp/event_log"));
  ASSERT_NE(lst.find("description = foo bar"), std::string::npos);
  ASSERT_NE(lst.find("notes = svc notes"), std::string::npos);
  ASSERT_NE(lst.find("notes_url = svc notes url"), std::string::npos);
  ASSERT_NE(lst.find("action_url = svc action url"), std::string::npos);
  ASSERT_NE(lst.find("host_id = 1"), std::string::npos);
  ASSERT_NE(lst.find("service_id = 2"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, BrokerEventJsonEncode) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->service_description = "foo bar";
  svc->notes = "svc notes";
  svc->notes_url = "d:\\\\bonjour le \"monde\"";
  svc->action_url = "svc action url";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "broker_api_version=2\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  broker_log:info(0, broker.json_encode(d))\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(svc);
  std::string lst(ReadFile("/tmp/event_log"));
  ASSERT_NE(
      lst.find(
          "{ \"_type\": 65559, \"category\": 1, \"element\": 23, "
          "\"acknowledged\":false, \"acknowledgement_type\":0, "
          "\"action_url\":\"svc action url\", \"active_checks\":false, "
          "\"check_freshness\":false, \"check_interval\":0, "
          "\"check_period\":\"\", \"check_type\":0, \"check_attempt\":0, "
          "\"state\":4, \"default_active_checks\":false, "
          "\"default_event_handler_enabled\":false, "
          "\"default_flap_detection\":false, \"default_notify\":false, "
          "\"default_passive_checks\":false, \"scheduled_downtime_depth\":0, "
          "\"display_name\":\"\", \"enabled\":true, \"event_handler\":\"\", "
          "\"event_handler_enabled\":false, \"execution_time\":0, "
          "\"first_notification_delay\":0, \"flap_detection\":false, "
          "\"flap_detection_on_critical\":false, "
          "\"flap_detection_on_ok\":false, "
          "\"flap_detection_on_unknown\":false, "
          "\"flap_detection_on_warning\":false, \"freshness_threshold\":0, "
          "\"checked\":false, \"high_flap_threshold\":0, \"host_id\":1, "
          "\"icon_image\":\"\", \"icon_image_alt\":\"\", \"service_id\":2, "
          "\"flapping\":false, \"volatile\":false, \"last_hard_state\":4, "
          "\"latency\":0, \"low_flap_threshold\":0, "
          "\"max_check_attempts\":0, \"no_more_notifications\":false, "
          "\"notes\":\"svc notes\", \"notes_url\":\"d:\\\\\\\\bonjour le "
          "\\\"monde\\\"\", \"notification_interval\":0, "
          "\"notification_number\":0, \"notification_period\":\"\", "
          "\"notify\":false, \"notify_on_critical\":false, "
          "\"notify_on_downtime\":false, \"notify_on_flapping\":false, "
          "\"notify_on_recovery\":false, \"notify_on_unknown\":false, "
          "\"notify_on_warning\":false, \"obsess_over_service\":false, "
          "\"passive_checks\":false, \"percent_state_change\":0, "
          "\"retry_interval\":0, \"description\":\"foo bar\", "
          "\"should_be_scheduled\":false, \"stalk_on_critical\":false, "
          "\"stalk_on_ok\":false, \"stalk_on_unknown\":false, "
          "\"stalk_on_warning\":false, \"state_type\":0, "
          "\"check_command\":\"\", \"output\":\"\", \"perfdata\":\"\", "
          "\"retain_nonstatus_information\":false, "
          "\"retain_status_information\":false}"),
      std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, TestHostApiV1) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->host_name = "foo bar host cache";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "broker_api_version=1\n\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  local hst = broker_cache:get_host(1)\n"
               "  broker_log:info(0, 'type of d = ' .. type(d))\n"
               "  broker_log:info(0, 'type of hst = ' .. type(hst))\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(hst);
  std::string lst(ReadFile("/tmp/event_log"));
  std::cout << lst << std::endl;
  ASSERT_NE(lst.find("type of d = table"), std::string::npos);
  ASSERT_NE(lst.find("type of hst = table"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, TestHostApiV2) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::host> hst(new neb::host);
  hst->host_id = 1;
  hst->host_name = "foo bar host cache";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "broker_api_version=2\n\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  local hst = broker_cache:get_host(1)\n"
               "  broker_log:info(0, 'type of d = ' .. type(d))\n"
               "  broker_log:info(0, 'type of hst = ' .. type(hst))\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(hst);
  std::string lst(ReadFile("/tmp/event_log"));
  std::cout << lst << std::endl;
  ASSERT_NE(lst.find("type of d = userdata"), std::string::npos);
  ASSERT_NE(lst.find("type of hst = userdata"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, TestSvcApiV2) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->service_description = "foo bar cache";
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "broker_api_version='2'\n\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  local svc = broker_cache:get_service(1, 2)\n"
               "  broker_log:info(0, 'type of d = ' .. type(d))\n"
               "  broker_log:info(0, 'type of svc = ' .. type(svc))\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(svc);
  std::string lst(ReadFile("/tmp/event_log"));
  std::cout << lst << std::endl;
  ASSERT_NE(lst.find("type of d = userdata"), std::string::npos);
  ASSERT_NE(lst.find("type of svc = userdata"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, TestSvcApiV1) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->service_description = "foo bar cache";
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "broker_api_version=1\n\n"
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  local svc = broker_cache:get_service(1, 2)\n"
               "  broker_log:info(0, 'type of d = ' .. type(d))\n"
               "  broker_log:info(0, 'type of svc = ' .. type(svc))\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(svc);
  std::string lst(ReadFile("/tmp/event_log"));
  std::cout << lst << std::endl;
  ASSERT_NE(lst.find("type of d = table"), std::string::npos);
  ASSERT_NE(lst.find("type of svc = table"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

TEST_F(LuaTest, BrokerEventCache) {
  modules::handle h("./neb/10-neb.so");
  std::map<std::string, misc::variant> conf;
  std::shared_ptr<neb::service> svc(new neb::service);
  svc->host_id = 1;
  svc->service_id = 2;
  svc->service_description = "foo bar cache";
  svc->notes = "svc notes";
  svc->notes_url = "svc notes url";
  svc->action_url = "svc action url";
  std::string filename("/tmp/cache_test.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/event_log')\n"
               "end\n\n"
               "function write(d)\n"
               "  local svc = broker_cache:get_service(1, 2)\n"
               "  broker_log:info(0, 'description = ' .. svc.description)\n"
               "end\n");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  binding->write(svc);
  std::string lst(ReadFile("/tmp/event_log"));
  ASSERT_NE(lst.find("description = foo bar cache"), std::string::npos);
  RemoveFile(filename);
  RemoveFile("/tmp/event_log");
}

// When the user needs the md5 sum of a string, he can use the md5 function
// that returns it as a string with hexadecimal number.
TEST_F(LuaTest, md5) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/md5.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local info = broker.md5('Hello World!')\n"
               "  broker_log:info(1, info)\n"
               "end\n"
               "function write(d)\n"
               "  return true\n"
               "end");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));
  ASSERT_NE(result.find("ed076287532e86365e841e92bfc50d8c"), std::string::npos);

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}

// When the user needs the md5 sum of a string, he can use the md5 function
// that returns it as a string with hexadecimal number.
TEST_F(LuaTest, emptyMd5) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/md5.lua");
  CreateScript(filename,
               "function init(conf)\n"
               "  broker_log:set_parameters(3, '/tmp/log')\n"
               "  local info = broker.md5('')\n"
               "  broker_log:info(1, info)\n"
               "end\n"
               "function write(d)\n"
               "  return true\n"
               "end");
  std::unique_ptr<luabinding> binding(new luabinding(filename, conf, *_cache));
  std::string result(ReadFile("/tmp/log"));
  ASSERT_NE(result.find("d41d8cd98f00b204e9800998ecf8427e"), std::string::npos);

  RemoveFile(filename);
  RemoveFile("/tmp/log");
}
