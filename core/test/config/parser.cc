/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include "com/centreon/broker/config/parser.hh"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/exceptions/msg_fmt.hh"
#include <nlohmann/json.hpp>

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace nlohmann;

/**
 *  Check that 'input' and 'output' are properly parsed by the
 *  configuration parser.
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST(parser, endpoint) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);

  // Data.
  std::string data{
      "\n{"
      "  \"centreonBroker\": {\n"
      "    \"input\": {\n"
      "      \"name\": \"CentreonInput\",\n"
      "      \"type\": \"tcp\",\n"
      "      \"port\": \"5668\",\n"
      "      \"protocol\": \"ndo\",\n"
      "      \"compression\": \"yes\"\n"
      "    },\n"
      "    \"output\": [\n"
      "      {\n"
      "        \"name\": \"CentreonDatabase\",\n"
      "        \"type\": \"sql\",\n"
      "        \"db_type\": \"mysql\",\n"
      "        \"db_host\": \"localhost\",\n"
      "        \"db_port\": \"3306\",\n"
      "        \"db_user\": \"centreon\",\n"
      "        \"db_password\": \"merethis\",\n"
      "        \"db_name\": \"centreon_storage\",\n"
      "        \"failover\": \"CentreonRetention\",\n"
      "        \"secondary_failover\": [\n"
      "          \"CentreonSecondaryFailover1\",\n"
      "          \"CentreonSecondaryFailover2\"\n"
      "        ],\n"
      "        \"buffering_timeout\": \"10\",\n"
      "        \"read_timeout\": \"5\",\n"
      "        \"retry_interval\": \"300\",\n"
      "        \"f\": \"1\"\n"
      "      },\n"
      "      {\n"
      "        \"name\": \"CentreonRetention\",\n"
      "        \"type\": \"file\",\n"
      "        \"path\": \"retention.dat\",\n"
      "        \"protocol\": \"ndo\"\n"
      "      },\n"
      "      {\n"
      "        \"name\": \"CentreonSecondaryFailover1\",\n"
      "        \"type\": \"file\",\n"
      "        \"path\": \"retention.dat\",\n"
      "        \"protocol\": \"ndo\"\n"
      "      },\n"
      "      {\n"
      "        \"name\": \"CentreonSecondaryFailover2\",\n"
      "        \"type\": \"file\",\n"
      "        \"path\": \"retention.dat\",\n"
      "        \"protocol\": \"ndo\"\n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "}\n"};

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  config::state s{p.parse(config_file)};

  // Remove temporary file.
  ::remove(config_file.c_str());

  // Check against expected result.
  ASSERT_EQ(s.loggers().size(), 0u);
  ASSERT_EQ(s.endpoints().size(), 5u);

  // Check input #1.
  std::list<config::endpoint>::iterator it(s.endpoints().begin());
  config::endpoint input1(*(it++));
  ASSERT_EQ(input1.name, "CentreonInput");
  ASSERT_EQ(input1.type, "tcp");
  ASSERT_EQ(input1.params["port"], "5668");
  ASSERT_EQ(input1.params["protocol"], "ndo");
  ASSERT_EQ(input1.params["compression"], "yes");

  // Check output #1.
  config::endpoint output1(*(it++));
  ASSERT_EQ(output1.name, "CentreonDatabase");
  ASSERT_EQ(output1.type, "sql");
  ASSERT_EQ(output1.failovers.size(), 1u);
  ASSERT_EQ(output1.failovers.front(), "CentreonRetention");
  ASSERT_EQ(output1.buffering_timeout, 10);
  ASSERT_EQ(output1.read_timeout, 5);
  ASSERT_EQ(output1.retry_interval, 300);
  ASSERT_EQ(output1.params["db_type"], "mysql");
  ASSERT_EQ(output1.params["db_host"], "localhost");
  ASSERT_EQ(output1.params["db_port"], "3306");
  ASSERT_EQ(output1.params["db_user"], "centreon");
  ASSERT_EQ(output1.params["db_password"], "merethis");
  ASSERT_EQ(output1.params["db_name"], "centreon_storage");

  // Check output #2.
  config::endpoint output2(*(it++));
  ASSERT_EQ(output2.name, "CentreonRetention");
  ASSERT_EQ(output2.type, "file");
  ASSERT_EQ(output2.params["path"], "retention.dat");
  ASSERT_EQ(output2.params["protocol"], "ndo");

  // Check output #3.
  config::endpoint output3(*(it++));
  ASSERT_EQ(output3.name, "CentreonSecondaryFailover1");
  ASSERT_EQ(output3.type, "file");
  ASSERT_EQ(output3.params["path"], "retention.dat");
  ASSERT_EQ(output3.params["protocol"], "ndo");

  // Check output #4.
  config::endpoint output4(*it);
  ASSERT_EQ(output4.name, "CentreonSecondaryFailover2");
  ASSERT_EQ(output4.type, "file");
  ASSERT_EQ(output4.params["path"], "retention.dat");
  ASSERT_EQ(output4.params["protocol"], "ndo");
}

/**
 *  Check that 'logger's are properly parsed by the configuration
 *  parser.
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST(parser, logger) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "    \"logger\": [\n"
      "      {\n"
      "        \"type\": \"file\",\n"
      "        \"name\": \"my_log_file\",\n"
      "        \"config\": true,\n"
      "        \"debug\": false,\n"
      "        \"error\": true,\n"
      "        \"info\": false,\n"
      "        \"level\": \"2\"\n"
      "      },"
      "      {"
      "        \"type\": \"standard\",\n"
      "        \"name\": \"stderr\",\n"
      "        \"config\": false,\n"
      "        \"error\": true,\n"
      "        \"debug\": false,\n"
      "        \"info\": true,\n"
      "        \"level\": \"3\"\n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  std::ifstream f(config_file);
  std::string const& json_to_parse{std::istreambuf_iterator<char>(f),
                                   std::istreambuf_iterator<char>()};

  json doc = json::parse(json_to_parse);

  // Parse.
  config::parser p;
  config::state s{p.parse(config_file)};

  // Remove temporary file.
  ::remove(config_file.c_str());

  // Check against expected result.
  ASSERT_EQ(s.loggers().size(), 2u);

  // Check logger #1.
  std::list<config::logger>::iterator it(s.loggers().begin());
  config::logger l1(*(it++));
  ASSERT_EQ(l1.type(), config::logger::file);
  ASSERT_EQ(l1.name(), "my_log_file");
  ASSERT_EQ(l1.config(), true);
  ASSERT_EQ(l1.debug(), false);
  ASSERT_EQ(l1.error(), true);
  ASSERT_EQ(l1.info(), false);
  ASSERT_EQ(l1.level(), 2);

  // Check logger #2.
  config::logger l2(*it);
  ASSERT_EQ(l2.type(), config::logger::standard);
  ASSERT_EQ(l2.name(), "stderr");
  ASSERT_EQ(l2.config(), false);
  ASSERT_EQ(l2.debug(), false);
  ASSERT_EQ(l2.error(), true);
  ASSERT_EQ(l2.info(), true);
  ASSERT_EQ(l2.level(), 3);
}

/**
 *  Check that 'logger's are properly parsed by the configuration
 *  parser.
 *
 *  @return EXIT_SUCCESS on success.
 */
TEST(parser, global) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": []\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  ASSERT_THROW(p.parse(config_file), std::exception);
}

TEST(parser, log) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": {\n"
      "       \"directory\": \"/tmp\"\n"
      "     }\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  config::state s{p.parse(config_file)};

  // Remove temporary file.
  ::remove(config_file.c_str());

  // Check global params
  ASSERT_EQ(s.rpc_port(), 0);
  ASSERT_EQ(s.broker_id(), 1);
  ASSERT_EQ(s.broker_name(), "central-broker-master");
  ASSERT_EQ(s.poller_id(), 1);
  ASSERT_EQ(s.module_directory(), "/usr/share/centreon/lib/centreon-broker");
  ASSERT_EQ(s.log_timestamp(), true);
  ASSERT_EQ(s.log_thread_id(), false);
  ASSERT_EQ(s.event_queue_max_size(), 100000);
  ASSERT_EQ(s.command_file(), "/var/lib/centreon-broker/command.sock");
  ASSERT_EQ(s.cache_directory(), "/var/lib/centreon-broker/");
  ASSERT_EQ(s.log_conf().directory, "/tmp");
  ASSERT_EQ(s.log_conf().max_size, 0u);
}

TEST(parser, logBadFilename) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": {\n"
      "       \"filename\": \"toto/titi\"\n"
      "     }\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  ASSERT_THROW(p.parse(config_file), msg_fmt);

  // Remove temporary file.
  ::remove(config_file.c_str());
}

TEST(parser, logDefaultDir) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": {\n"
      "       \"filename\": \"toto\",\n"
      "       \"max_size\": \"12345\",\n"
      "       \"loggers\": {\n"
      "         \"tcp\": \"warning\",\n"
      "         \"bam\": \"critical\"\n"
      "       }\n"
      "     }\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  config::state s{p.parse(config_file)};

  // Remove temporary file.
  ::remove(config_file.c_str());
  ASSERT_EQ(s.log_conf().directory, "/var/log/centreon-broker");
  ASSERT_EQ(s.log_conf().filename, "toto");
  ASSERT_EQ(s.log_conf().max_size, 12345u);
  ASSERT_EQ(s.log_conf().loggers.size(), 2u);
}

TEST(parser, logBadMaxSize) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": {\n"
      "       \"filename\": \"toto\"\n"
      "       \"max_size\": \"12a345\"\n"
      "     }\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  ASSERT_THROW(p.parse(config_file), msg_fmt);

  // Remove temporary file.
  ::remove(config_file.c_str());
}

TEST(parser, logBadLoggers) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": {\n"
      "       \"filename\": \"toto\"\n"
      "       \"max_size\": \"12345\"\n"
      "       \"loggers\": []\n"
      "     }\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  ASSERT_THROW(p.parse(config_file), msg_fmt);

  // Remove temporary file.
  ::remove(config_file.c_str());
}

TEST(parser, logBadLogger) {
  // File name.
  std::string config_file(misc::temp_path());

  // Open file.
  FILE* file_stream(fopen(config_file.c_str(), "w"));
  if (!file_stream)
    throw msg_fmt("could not open '{}'", config_file);
  // Data.
  std::string data;
  data =
      "{\n"
      "  \"centreonBroker\": {\n"
      "     \"broker_id\": 1,\n"
      "     \"broker_name\": \"central-broker-master\",\n"
      "     \"poller_id\": 1,\n"
      "     \"poller_name\": \"Central\",\n"
      "     \"module_directory\": "
      "\"/usr/share/centreon/lib/centreon-broker\",\n"
      "     \"log_timestamp\": true,\n"
      "     \"event_queue_max_size\": 100000,\n"
      "     \"command_file\": \"/var/lib/centreon-broker/command.sock\",\n"
      "     \"cache_directory\": \"/var/lib/centreon-broker\",\n"
      "     \"log_thread_id\": false,\n"
      "     \"log\": {\n"
      "       \"filename\": \"toto\"\n"
      "       \"max_size\": \"12345\"\n"
      "       \"loggers\": { \"minou\": \"trace\" }\n"
      "     }\n"
      "  }\n"
      "}\n";

  // Write data.
  if (fwrite(data.c_str(), data.size(), 1, file_stream) != 1)
    throw msg_fmt("could not write content of '{}'", config_file);

  // Close file.
  fclose(file_stream);

  // Parse.
  config::parser p;
  ASSERT_THROW(p.parse(config_file), msg_fmt);

  // Remove temporary file.
  ::remove(config_file.c_str());
}
