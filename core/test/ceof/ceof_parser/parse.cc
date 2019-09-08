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
#include <gtest/gtest.h>
#include "com/centreon/broker/ceof/ceof_parser.hh"

using namespace com::centreon::broker;

// Given a ceof_parser object
// When it is constructed with a valid configuration string
// Then parse() returns a valid ceof_iterator
TEST(CeofCeofParserParse, Valid1) {
  ceof::ceof_parser p(
      "define command {\n"
      "    command_name                   centreon-discovery-nmap \n"
      "    command_line                   perl "
      "/usr/lib/nagios/plugins/centreon-plugins/centreon_plugins.pl "
      "--plugin=discovery::nmap::plugin --mode=fastscan --range=\"$NETWORK$\" "
      "\n"
      "}\n"
      "define command {\n"
      "    command_name                   centreon-discovery-toto\n"
      "    command_line                   "
      "/usr/lib/nagios/plugins/check_centreon_dummy\n"
      "}\n");
  ceof::ceof_iterator it(p.parse());
  ASSERT_EQ(it.get_value(), "command");
  ceof::ceof_iterator child(it.enter_children());
  ASSERT_EQ(child.get_value(), "command_name");
  ASSERT_EQ((++child).get_value(), "centreon-discovery-nmap");
  ASSERT_EQ((++child).get_value(), "command_line");
  ASSERT_EQ(
      (++child).get_value(),
      "perl /usr/lib/nagios/plugins/centreon-plugins/centreon_plugins.pl "
      "--plugin=discovery::nmap::plugin --mode=fastscan --range=\"$NETWORK$\"");
  ASSERT_TRUE((++child).end());
  ++it;
  ASSERT_EQ(it.get_value(), "command");
  child = it.enter_children();
  ASSERT_EQ(child.get_value(), "command_name");
  ASSERT_EQ((++child).get_value(), "centreon-discovery-toto");
  ASSERT_EQ((++child).get_value(), "command_line");
  ASSERT_EQ((++child).get_value(),
            "/usr/lib/nagios/plugins/check_centreon_dummy");
  ASSERT_TRUE((++child).end());
  ++it;
  ASSERT_TRUE(it.end());
}
