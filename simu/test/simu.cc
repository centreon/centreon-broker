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
                         "return { _type=65559,\n"
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

// When a script read() function returns a table
// And the table is an event
// Then no exception is thrown
// And read() returns true
TEST_F(SimuGenericTest, ReadReturnCustomVariable) {
  std::string filename("/tmp/good_read.lua");
  CreateScript(filename, "function init()\n"
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
  QMap<QString, QVariant> conf;
  modules::loader l;
  l.load_file("./neb/10-neb.so");
  std::auto_ptr<luabinding> lb(new luabinding(
                     filename,
                     conf));
  misc::shared_ptr<io::data> d;
  ASSERT_TRUE(lb->read(d));
  RemoveFile(filename);
  neb::custom_variable* cv(static_cast<neb::custom_variable*>(d.data()));
  ASSERT_TRUE(cv->type() == 65539);
  ASSERT_EQ(cv->host_id, 31);
  ASSERT_TRUE(cv->enabled);
}
