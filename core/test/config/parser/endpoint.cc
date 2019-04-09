/*
** Copyright 2012,2015-2019 Centreon
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
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker;

class ConfigParser : public ::testing::Test {
 public:
  void SetUp() {
    // File name.
    std::string config_file(misc::temp_path());
    _create_conf_file(config_file);

    // Parse.
    config::parser p;
    p.parse(config_file.c_str(), _state);
  }

  void _create_conf_file(std::string const& config_file) {
    // Open file.
    FILE* file_stream(fopen(config_file.c_str(), "w"));
    if (!file_stream)
      throw (exceptions::msg() << "could not open '"
        << config_file.c_str() << "'");

    // Data.
    std::string data(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        "<centreonbroker>\n"
        " <broker_id><![CDATA[3]]></broker_id>\n"
        " <broker_name><![CDATA[central-module-master]]></broker_name>\n"
        "  <custom_variable_filter>\n"
        "   <enabled><![CDATA[yes]]></enabled>\n"
        "   <white_list>\n"
        "    <cv><![CDATA[WARNING]]></cv>\n"
        "    <cv><![CDATA[CRITICAL]]></cv>\n"
        "   </white_list>\n"
        "  </custom_variable_filter>\n"
        "  <poller_id><![CDATA[1]]></poller_id>\n"
        "  <input>\n"
        "    <name>CentreonInput</name>\n"
        "    <type>tcp</type>\n"
        "    <port>5668</port>\n"
        "    <protocol>ndo</protocol>\n"
        "    <compression>yes</compression>\n"
        "  </input>\n"
        "  <output>\n"
        "    <name>CentreonDatabase</name>\n"
        "    <type>sql</type>\n"
        "    <db_type>mysql</db_type>\n"
        "    <db_host>localhost</db_host>\n"
        "    <db_port>3306</db_port>\n"
        "    <db_user>centreon</db_user>\n"
        "    <db_password>merethis</db_password>\n"
        "    <db_name>centreon_storage</db_name>\n"
        "    <failover>CentreonRetention</failover>\n"
        "    <secondary_failover>CentreonSecondaryFailover1</secondary_failover>\n"
        "    <secondary_failover>CentreonSecondaryFailover2</secondary_failover>\n"
        "    <buffering_timeout>10</buffering_timeout>\n"
        "    <read_timeout>5</read_timeout>\n"
        "    <retry_interval>300</retry_interval>\n"
        "  </output>\n"
        "  <output>\n"
        "    <name>CentreonRetention</name>\n"
        "    <type>file</type>\n"
        "    <path>retention.dat</path>\n"
        "    <protocol>ndo</protocol>\n"
        "  </output>\n"
        "  <output>\n"
        "    <name>CentreonSecondaryFailover1</name>\n"
        "    <type>file</type>\n"
        "    <path>retention.dat</path>\n"
        "    <protocol>ndo</protocol>\n"
        "  </output>\n"
        "  <output>\n"
        "    <name>CentreonSecondaryFailover2</name>\n"
        "    <type>file</type>\n"
        "    <path>retention.dat</path>\n"
        "    <protocol>ndo</protocol>\n"
        "  </output>\n"
        "</centreonbroker>\n");

    // Write data.
    if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
      throw (exceptions::msg() << "could not write content of '"
        << config_file.c_str() << "'");

    // Close file.
    fclose(file_stream);
  }
 protected:
  config::state _state;
};

TEST_F(ConfigParser, BasicConfig) {
  ASSERT_EQ(_state.loggers().size(), 0);
  ASSERT_EQ(_state.endpoints().size(), 5);
}

TEST_F(ConfigParser, TcpConfig) {
  std::list<config::endpoint>::iterator it(_state.endpoints().begin());
  config::endpoint input1(*(it));
  ASSERT_EQ(input1.name, "CentreonInput");
  ASSERT_EQ(input1.type, "tcp");
  ASSERT_EQ(input1.params["port"], "5668");
  ASSERT_EQ(input1.params["protocol"], "ndo");
  ASSERT_EQ(input1.params["compression"], "yes");
}

TEST_F(ConfigParser, SqlConfig) {
  std::list<config::endpoint>::iterator it(_state.endpoints().begin());
  ++it;
  config::endpoint output1(*it);
  ASSERT_EQ(output1.name, "CentreonDatabase");
  ASSERT_EQ(output1.type, "sql");
  ASSERT_EQ(output1.failovers.size(), 3);
  ASSERT_EQ(std::count(
                 output1.failovers.begin(),
                 output1.failovers.end(),
                 "CentreonRetention"), 1);
  ASSERT_EQ(std::count(
                 output1.failovers.begin(),
                 output1.failovers.end(),
                 "CentreonSecondaryFailover1"), 1);
  ASSERT_EQ(std::count(
                 output1.failovers.begin(),
                 output1.failovers.end(),
                 "CentreonSecondaryFailover2"), 1);
  ASSERT_EQ(output1.buffering_timeout, 10);
  ASSERT_EQ(output1.read_timeout, 5);
  ASSERT_EQ(output1.retry_interval, 300);
  ASSERT_EQ(output1.params["db_type"], "mysql");
  ASSERT_EQ(output1.params["db_host"], "localhost");
  ASSERT_EQ(output1.params["db_port"], "3306");
  ASSERT_EQ(output1.params["db_user"], "centreon");
  ASSERT_EQ(output1.params["db_password"], "merethis");
  ASSERT_EQ(output1.params["db_name"], "centreon_storage");
}

TEST_F(ConfigParser, CentreonRetention) {
  std::list<config::endpoint>::iterator it(_state.endpoints().begin());
  ++it; ++it;
  config::endpoint output2(*it);
  ASSERT_EQ(output2.name, "CentreonRetention");
  ASSERT_EQ(output2.type, "file");
  ASSERT_EQ(output2.params["path"], "retention.dat");
  ASSERT_EQ(output2.params["protocol"], "ndo");
}

TEST_F(ConfigParser, CentreonSecondaryFailover1) {
  std::list<config::endpoint>::iterator it(_state.endpoints().begin());
  ++it; ++it; ++it;
  config::endpoint output3(*it);
  ASSERT_EQ(output3.name, "CentreonSecondaryFailover1");
  ASSERT_EQ(output3.type, "file");
  ASSERT_EQ(output3.params["path"], "retention.dat");
  ASSERT_EQ(output3.params["protocol"], "ndo");
}

TEST_F(ConfigParser, CentreonSecondaryFailover2) {
  std::list<config::endpoint>::iterator it(_state.endpoints().begin());
  ++it; ++it; ++it; ++it;
  config::endpoint output4(*it);
  ASSERT_EQ(output4.name, "CentreonSecondaryFailover2");
  ASSERT_EQ(output4.type, "file");
  ASSERT_EQ(output4.params["path"], "retention.dat");
  ASSERT_EQ(output4.params["protocol"], "ndo");
}

TEST_F(ConfigParser, CustomVariableFilterActive) {
  ASSERT_TRUE(_state.custom_variable_filter_enabled());
}

TEST_F(ConfigParser, CustomVariableFilter) {
  std::set<std::string>::iterator it(_state.custom_variable_filter().begin());
  ASSERT_EQ(_state.custom_variable_filter().size(), 2);
  ASSERT_EQ(*(it++), "CRITICAL");
  ASSERT_EQ(*(it++), "WARNING");
}
