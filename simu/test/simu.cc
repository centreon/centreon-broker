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
#include <fstream>
#include <list>
#include <memory>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/modules/handle.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/simu/luabinding.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::simu;

class SimuGenericTest : public ::testing::Test {
 public:
  void SetUp() override {
    try {
      config::applier::init(0, "test_broker");
    } catch (std::exception const& e) {
      (void)e;
    }
  }
  void TearDown() override { config::applier::deinit(); }

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
};

// When a lua script that does not exist is loaded
// Then an exception is thrown
TEST_F(SimuGenericTest, MissingScript) {
  std::map<std::string, misc::variant> conf;
  ASSERT_THROW(new luabinding("/tmp/this_script_does_not_exist.lua", conf),
               msg_fmt);
}

// When a lua script with error such as number divided by nil is loaded
// Then an exception is thrown
TEST_F(SimuGenericTest, FaultyScript) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/faulty.lua");
  CreateScript(filename,
               "local a = { 1, 2, 3 }\n"
               "local b = 18 / a[4]");
  ASSERT_THROW(new luabinding(filename, conf), msg_fmt);
  RemoveFile(filename);
}

// When a lua script that does not contain an init() function is loaded
// Then an exception is thrown
TEST_F(SimuGenericTest, WithoutInit) {
  std::map<std::string, misc::variant> conf;
  std::string filename("/tmp/without_init.lua");
  CreateScript(filename, "local a = { 1, 2, 3 }\n");
  ASSERT_THROW(new luabinding(filename, conf), msg_fmt);
  RemoveFile(filename);
}

// When a json parameters file exists but the lua script is incomplete
// Then an exception is thrown
TEST_F(SimuGenericTest, IncompleteScript) {
  std::string filename("/tmp/bad_init.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "local a = { 1, 2, 3 }\n");
  std::map<std::string, misc::variant> conf;
  ASSERT_THROW(new luabinding(filename, conf), msg_fmt);
  RemoveFile(filename);
}

// When a script read() function does not return nil or a Lua table
// Then an exception is thrown
TEST_F(SimuGenericTest, ReadReturnValue1) {
  std::string filename("/tmp/bad_read.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "function read()\n"
               "return 2\n"
               "end\n");
  std::map<std::string, misc::variant> conf;
  std::unique_ptr<luabinding> lb(new luabinding(filename, conf));
  std::shared_ptr<io::data> d;
  ASSERT_THROW(lb->read(d), msg_fmt);
  RemoveFile(filename);
}

// When a script read() function returns nil
// Then no exception is thrown
TEST_F(SimuGenericTest, ReadReturnValue2) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "function read()\n"
               "return nil\n"
               "end\n");
  std::map<std::string, misc::variant> conf;
  std::unique_ptr<luabinding> lb(new luabinding(filename, conf));
  std::shared_ptr<io::data> d;
  ASSERT_FALSE(lb->read(d));
  RemoveFile(filename);
}

// When a script read() function returns a table
// And the table does not represent an event
// Then no exception is thrown
// And read() returns false
TEST_F(SimuGenericTest, ReadReturnValue3) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "function read()\n"
               "return { a='toto' }\n"
               "end\n");
  std::map<std::string, misc::variant> conf;
  std::unique_ptr<luabinding> lb(new luabinding(filename, conf));
  std::shared_ptr<io::data> d;
  ASSERT_FALSE(lb->read(d));
  RemoveFile(filename);
}

// When a script read() function returns a table
// And the table is an event
// Then no exception is thrown
// And read() returns true
TEST_F(SimuGenericTest, ReadReturnValue4) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "function read()\n"
               "return { _type=65559,\n"
               "service_id=1,\n"
               "ctime=1536659255,\n"
               "host_id=2,\n"
               "element=23,\n"
               "category=1,\n"
               "description=\"Super description\"\n"
               "}\n"
               "end\n");
  std::map<std::string, misc::variant> conf;
  modules::handle h("./neb/10-neb.so");
  std::unique_ptr<luabinding> lb(new luabinding(filename, conf));
  std::shared_ptr<io::data> d;
  ASSERT_TRUE(lb->read(d));
  RemoveFile(filename);
  neb::service* svc(static_cast<neb::service*>(d.get()));
  ASSERT_TRUE(svc->type() == 65559);
  ASSERT_EQ(svc->host_id, 2U);
  std::cout << "service description: " << svc->service_description;
  ASSERT_TRUE(svc->service_description == "Super description");
}

// When a script read() function returns a table
// And the table is an event
// Then no exception is thrown
// And read() returns true
TEST_F(SimuGenericTest, ReadReturnCustomVariable) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename,
               "function init()\n"
               "end\n"
               "function read()\n"
               "  return {\n"
               "    service_id=498,\n"
               "    _type=65539,\n"
               "    update_time=1538146454,\n"
               "    modified=false,\n"
               "    host_id=31,\n"
               "    element=3,\n"
               "    name=\"PROCESSNAME\",\n"
               "    category=1,\n"
               "    value=\"centengine\",\n"
               "    default_value=\"centengine\"}\n"
               "end\n");
  std::map<std::string, misc::variant> conf;
  modules::handle h("./neb/10-neb.so");
  std::unique_ptr<luabinding> lb(new luabinding(filename, conf));
  std::shared_ptr<io::data> d;
  ASSERT_TRUE(lb->read(d));
  RemoveFile(filename);
  neb::custom_variable* cv(static_cast<neb::custom_variable*>(d.get()));
  ASSERT_TRUE(cv->type() == 65539);
  ASSERT_EQ(cv->host_id, 31U);
  ASSERT_TRUE(cv->enabled);
}
