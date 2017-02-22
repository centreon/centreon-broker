/*
** Copyright 2011-2013,2017 Centreon
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
#include <QList>
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

// Given an storage::parser object
// When parse_perfdata() is called with a valid perfdata string
// Then perfdata are returned in a list
TEST(StorageParserParsePerfdata, ParseSimple) {
  // Parse perfdata.
  QList<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata(
    "time=2.45698s;2.000000;5.000000;0.000000;10.000000",
    list);

  // Check list size.
  ASSERT_EQ(list.size(), 1);

  // Check content.
  QList<storage::perfdata>::const_iterator it(list.begin());
  storage::perfdata expected;
  expected.name("time");
  expected.value_type(storage::perfdata::gauge);
  expected.value(2.45698);
  expected.unit("s");
  expected.warning(2.0);
  expected.warning_low(0.0);
  expected.critical(5.0);
  expected.critical_low(0.0);
  expected.min(0.0);
  expected.max(10.0);
  ASSERT_TRUE(expected == *it);
}
