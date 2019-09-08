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

#include "com/centreon/broker/neb/set_log_data.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/neb/log_entry.hh"

using namespace com::centreon::broker;

// Engine stubs
uint64_t com::centreon::engine::get_host_id(std::string const& name) {
  return 1;
}

uint64_t com::centreon::engine::get_service_id(std::string const& host,
                                               std::string const& svc) {
  return 1;
}

TEST(SetLogData, Default) {
  // Log entry.
  neb::log_entry le;

  // Parse a service alert line.
  neb::set_log_data(le,
                    "EXTERNAL COMMAND: "
                    "SCHEDULE_FORCED_SVC_CHECK;MyHost;MyService;1428930446");

  // Check that it was properly parsed.
  ASSERT_FALSE(le.host_name != "");
  ASSERT_FALSE(le.log_type != 0);  // Default.
  ASSERT_FALSE(le.msg_type != 5);  // Default.
  ASSERT_FALSE(le.output !=
               "EXTERNAL COMMAND: "
               "SCHEDULE_FORCED_SVC_CHECK;MyHost;MyService;1428930446");
  ASSERT_FALSE(le.retry != 0);
  ASSERT_FALSE(le.service_description != "");
  ASSERT_FALSE(le.status != 0);
}

/**
 *  Check that a host alert log is properly parsed.
 *
 *  @return 0 on success.
 */
TEST(SetLogData, HostAlert) {
  // Log entry.
  neb::log_entry le;

  // Parse a host alert line.
  neb::set_log_data(
      le, "HOST ALERT: myserver;UNREACHABLE;HARD;4;Time to live exceeded");

  // Check that it was properly parsed.
  ASSERT_FALSE(le.host_name != "myserver");
  ASSERT_FALSE(le.log_type != 1);  // HARD
  ASSERT_FALSE(le.msg_type != 1);  // HOST ALERT
  ASSERT_FALSE(le.output != "Time to live exceeded");
  ASSERT_FALSE(le.retry != 4);
  ASSERT_FALSE(le.status != 2);  // UNREACHABLE
}

/**
 *  Check that a initial host state log is properly parsed.
 */
TEST(SetLogData, InititalHostState) {
  try {
    // #1
    {
      neb::log_entry le;
      neb::set_log_data(le, "INITIAL HOST STATE: myserver;UP;HARD;1;PING OK");
      ASSERT_FALSE(le.host_name != "myserver");
      ASSERT_FALSE(le.log_type != 1);  // HARD
      ASSERT_FALSE(le.msg_type != 9);  // INITIAL HOST STATE
      ASSERT_FALSE(le.output != "PING OK");
      ASSERT_FALSE(le.retry != 1);
      ASSERT_FALSE(le.status != 0);  // UP
    }

    // #2
    {
      neb::log_entry le;
      neb::set_log_data(le, "INITIAL HOST STATE: SERVER007;UNKNOWN;SOFT;2;");
      ASSERT_FALSE(le.host_name != "SERVER007");
      ASSERT_FALSE(le.log_type != 0);  // SOFT
      ASSERT_FALSE(le.msg_type != 9);  // INITIAL HOST STATE
      ASSERT_FALSE(le.output != "");
      ASSERT_FALSE(le.retry != 2);
      ASSERT_FALSE(le.status != 3);  // UNKNOWN
    }
  } catch (...) {
    ASSERT_TRUE(false);
  }
}

/**
 *  Check that an initial service state log is properly parsed.
 */
TEST(SetLogData, InititalServiceState) {
  // Log entry.
  neb::log_entry le;

  // Parse an initial service state line.
  neb::set_log_data(
      le,
      "INITIAL SERVICE STATE: myserver;myservice;UNKNOWN;SOFT;1;ERROR when "
      "getting SNMP version");

  // Check that it was properly parsed.
  ASSERT_FALSE(le.host_name != "myserver");
  ASSERT_FALSE(le.log_type != 0);  // SOFT
  ASSERT_FALSE(le.msg_type != 8);  // INITIAL SERVICE STATE
  ASSERT_FALSE(le.output != "ERROR when getting SNMP version");
  ASSERT_FALSE(le.retry != 1);
  ASSERT_FALSE(le.service_description != "myservice");
  ASSERT_FALSE(le.status != 3);  // UNKNOWN
}

/**
 *  Check that a service alert log is properly parsed.
 */
TEST(SetLogData, ServiceALert) {
  // Log entry.
  neb::log_entry le;

  // Parse a service alert line.
  neb::set_log_data(le,
                    "SERVICE ALERT: myserver;myservice;WARNING;SOFT;3;CPU 84%");

  // Check that it was properly parsed.
  ASSERT_FALSE(le.host_name != "myserver");
  ASSERT_FALSE(le.log_type != 0);
  ASSERT_FALSE(le.msg_type != 0);
  ASSERT_FALSE(le.output != "CPU 84%");
  ASSERT_FALSE(le.retry != 3);
  ASSERT_FALSE(le.service_description != "myservice");
  ASSERT_FALSE(le.status != 1);
}
