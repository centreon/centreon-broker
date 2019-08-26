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

#include <cmath>
#include <gtest/gtest.h>
#include <list>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

// Given a storage::parser object
// When parse_perfdata() is called with a valid perfdata string
// Then perfdata are returned in a list
TEST(StorageParserParsePerfdata, Simple1) {
  config::applier::init();
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  p.parse_perfdata(
    "time=2.45698s;2.000000;5.000000;0.000000;10.000000",
    lst);

  // Assertions.
  ASSERT_EQ(lst.size(), 1u);
  std::list<storage::perfdata>::const_iterator it(lst.begin());
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

TEST(StorageParserParsePerfdata, Simple2) {
  // Parse perfdata.
  std::list<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata("'ABCD12E'=18.00%;15:;10:;0;100", list);

  // Assertions.
  ASSERT_EQ(list.size(), 1u);
  std::list<storage::perfdata>::const_iterator it(list.begin());
  storage::perfdata expected;
  expected.name("ABCD12E");
  expected.value_type(storage::perfdata::gauge);
  expected.value(18.0);
  expected.unit("%");
  expected.warning(INFINITY);
  expected.warning_low(15.0);
  expected.critical(INFINITY);
  expected.critical_low(10.0);
  expected.min(0.0);
  expected.max(100.0);
  ASSERT_TRUE(expected == *it);
}

TEST(StorageParserParsePerfdata, Complex1) {
  // Parse perfdata.
  std::list<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata(
      "time=2.45698s;;nan;;inf d[metric]=239765B/s;5;;-inf; "
      "infotraffic=18x;;;; a[foo]=1234;10;11: c[bar]=1234;~:10;20:30 "
      "baz=1234;@10:20; 'q u x'=9queries_per_second;@10:;@5:;0;100",
      list);

  // Assertions.
  ASSERT_EQ(list.size(), 7u);
  std::list<storage::perfdata>::const_iterator it(list.begin());
  storage::perfdata expected;

  // #1.
  expected.name("time");
  expected.value_type(storage::perfdata::gauge);
  expected.value(2.45698);
  expected.unit("s");
  expected.max(INFINITY);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #2.
  expected = storage::perfdata();
  expected.name("metric");
  expected.value_type(storage::perfdata::derive);
  expected.value(239765);
  expected.unit("B/s");
  expected.warning(5.0);
  expected.warning_low(0.0);
  expected.min(-INFINITY);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #3.
  expected = storage::perfdata();
  expected.name("infotraffic");
  expected.value_type(storage::perfdata::gauge);
  expected.value(18.0);
  expected.unit("x");
  ASSERT_TRUE(expected == *it);
  ++it;

  // #4.
  expected = storage::perfdata();
  expected.name("foo");
  expected.value_type(storage::perfdata::absolute);
  expected.value(1234.0);
  expected.warning(10.0);
  expected.warning_low(0.0);
  expected.critical(INFINITY);
  expected.critical_low(11.0);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #5.
  expected = storage::perfdata();
  expected.name("bar");
  expected.value_type(storage::perfdata::counter);
  expected.value(1234.0);
  expected.warning(10.0);
  expected.warning_low(-INFINITY);
  expected.critical(30.0);
  expected.critical_low(20.0);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #6.
  expected = storage::perfdata();
  expected.name("baz");
  expected.value_type(storage::perfdata::gauge);
  expected.value(1234.0);
  expected.warning(20.0);
  expected.warning_low(10.0);
  expected.warning_mode(true);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #7.
  expected = storage::perfdata();
  expected.name("q u x");
  expected.value_type(storage::perfdata::gauge);
  expected.value(9.0);
  expected.unit("queries_per_second");
  expected.warning(INFINITY);
  expected.warning_low(10.0);
  expected.warning_mode(true);
  expected.critical(INFINITY);
  expected.critical_low(5.0);
  expected.critical_mode(true);
  expected.min(0.0);
  expected.max(100.0);
  ASSERT_TRUE(expected == *it);
}

// Given a storage::parser object
// When parse_perfdata() is called multiple time with valid strings
// Then the corresponding perfdata list is returned
TEST(StorageParserParsePerfdata, Loop) {
  // Objects.
  std::list<storage::perfdata> list;
  storage::parser p;

  // Loop.
  for (unsigned int i(0); i < 10000; ++i) {
    // Parse perfdata string.
    list.clear();
    p.parse_perfdata(
        "c[time]=2.45698s;2.000000;5.000000;0.000000;10.000000",
        list);

    // Assertions.
    ASSERT_EQ(list.size(), 1u);
    std::list<storage::perfdata>::const_iterator it(list.begin());
    storage::perfdata expected;
    expected.name("time");
    expected.value_type(storage::perfdata::counter);
    expected.value(2.45698);
    expected.unit("s");
    expected.warning(2.0);
    expected.warning_low(0.0);
    expected.critical(5.0);
    expected.critical_low(0.0);
    expected.min(0.0);
    expected.max(10.0);
    ASSERT_TRUE(expected == *it);
    ++it;
  }
}

// Given a storage::parser object
// When parse_perfdata() is called with an invalid string
// Then it throws a storage::exceptions::perfdata
TEST(StorageParserParsePerfdata, Incorrect1) {
  // Objects.
  std::list<storage::perfdata> list;
  storage::parser p;

  // Attempt to parse perfdata.
  ASSERT_THROW(
    { p.parse_perfdata("metric1= 10 metric2=42", list); },
    com::centreon::broker::storage::exceptions::perfdata);
}

// Given a storage::parser object
// When parse_perfdata() is called with a metric without value but with unit
// Then it throws a storage::exceptions::perfdata
TEST(StorageParserParsePerfdata, Incorrect2) {
  // Given
  std::list<storage::perfdata> list;
  storage::parser p;

  // Then
  ASSERT_THROW(
    { p.parse_perfdata("metric=kb/s", list); },
    com::centreon::broker::storage::exceptions::perfdata);
}

TEST(StorageParserParsePerfdata, LabelWithSpaces) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  p.parse_perfdata("  'foo  bar   '=2s;2;5;;", lst);

  // Assertions.
  ASSERT_EQ(lst.size(), 1u);
  std::list<storage::perfdata>::const_iterator it(lst.begin());
  storage::perfdata expected;
  expected.name("foo  bar");
  expected.value_type(storage::perfdata::gauge);
  expected.value(2);
  expected.unit("s");
  expected.warning(2.0);
  expected.warning_low(0.0);
  expected.critical(5.0);
  expected.critical_low(0.0);
  ASSERT_TRUE(expected == *it);
}

TEST(StorageParserParsePerfdata, Complex2) {
  // Parse perfdata.
  std::list<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata(
      "time=2,45698s;;nan;;inf d[metric]=239765B/s;5;;-inf; "
      "infotraffic=18,6x;;;; a[foo]=1234,17;10;11: c[bar]=1234,147;~:10;20:30",
      list);

  // Assertions.
  ASSERT_EQ(list.size(), 5u);
  std::list<storage::perfdata>::const_iterator it(list.begin());
  storage::perfdata expected;

  // #1.
  expected.name("time");
  expected.value_type(storage::perfdata::gauge);
  expected.value(2.45698);
  expected.unit("s");
  expected.max(INFINITY);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #2.
  expected = storage::perfdata();
  expected.name("metric");
  expected.value_type(storage::perfdata::derive);
  expected.value(239765);
  expected.unit("B/s");
  expected.warning(5.0);
  expected.warning_low(0.0);
  expected.min(-INFINITY);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #3.
  expected = storage::perfdata();
  expected.name("infotraffic");
  expected.value_type(storage::perfdata::gauge);
  expected.value(18.6);
  expected.unit("x");
  ASSERT_TRUE(expected == *it);
  ++it;

  // #4.
  expected = storage::perfdata();
  expected.name("foo");
  expected.value_type(storage::perfdata::absolute);
  expected.value(1234.17);
  expected.warning(10.0);
  expected.warning_low(0.0);
  expected.critical(INFINITY);
  expected.critical_low(11.0);
  ASSERT_TRUE(expected == *it);
  ++it;

  // #5.
  expected = storage::perfdata();
  expected.name("bar");
  expected.value_type(storage::perfdata::counter);
  expected.value(1234.147);
  expected.warning(10.0);
  expected.warning_low(-INFINITY);
  expected.critical(30.0);
  expected.critical_low(20.0);
  ASSERT_TRUE(expected == *it);
  ++it;
}
