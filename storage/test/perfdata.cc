/*
 * Copyright 2011-2021 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/storage/exceptions/perfdata.hh"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <cmath>
#include <list>

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

TEST(StoragePerfdataException, All) {
  bool success{false};
  try {
    throw storage::exceptions::perfdata() << "test exception";
  } catch (storage::exceptions::perfdata const& e) {
    try {
      e.rethrow();
    } catch (storage::exceptions::perfdata const& e) {
      exceptions::msg* msg{e.clone()};
      if (std::string{msg->what()} == "test exception")
        success = true;
      delete msg;
    }
  }

  ASSERT_TRUE(success);
}

/**
 *  Check that the perfdata assignment operator works properly.
 */
TEST(StoragePerfdata, Assign) {
  // First object.
  storage::perfdata p1;
  p1.critical(42.0);
  p1.critical_low(-456.032);
  p1.critical_mode(false);
  p1.max(76.3);
  p1.min(567.2);
  p1.name("foo");
  p1.unit("bar");
  p1.value(52189.912);
  p1.value_type(storage::perfdata::counter);
  p1.warning(4548.0);
  p1.warning_low(42.42);
  p1.warning_mode(true);

  // Second object.
  storage::perfdata p2;
  p2.critical(2345678.9672374);
  p2.critical_low(-3284523786.8923);
  p2.critical_mode(true);
  p2.max(834857.9023);
  p2.min(348.239479);
  p2.name("merethis");
  p2.unit("centreon");
  p2.value(8374598345.234);
  p2.value_type(storage::perfdata::absolute);
  p2.warning(0.823745784);
  p2.warning_low(NAN);
  p2.warning_mode(false);

  // Assignment.
  p2 = p1;

  // Change first object.
  p1.critical(9432.5);
  p1.critical_low(1000.0001);
  p1.critical_mode(true);
  p1.max(123.0);
  p1.min(843.876);
  p1.name("baz");
  p1.unit("qux");
  p1.value(3485.9);
  p1.value_type(storage::perfdata::derive);
  p1.warning(3612.0);
  p1.warning_low(-987579.0);
  p1.warning_mode(false);

  // Check objects properties values.
  ASSERT_FALSE(fabs(p1.critical() - 9432.5) > 0.00001);
  ASSERT_FALSE(fabs(p1.critical_low() - 1000.0001) > 0.00001);
  ASSERT_FALSE(!p1.critical_mode());
  ASSERT_FALSE(fabs(p1.max() - 123.0) > 0.00001);
  ASSERT_FALSE(fabs(p1.min() - 843.876) > 0.00001);
  ASSERT_FALSE(p1.name() != "baz");
  ASSERT_FALSE(p1.unit() != "qux");
  ASSERT_FALSE(fabs(p1.value() - 3485.9) > 0.00001);
  ASSERT_FALSE(p1.value_type() != storage::perfdata::derive);
  ASSERT_FALSE(fabs(p1.warning() - 3612.0) > 0.00001);
  ASSERT_FALSE(fabs(p1.warning_low() + 987579.0) > 0.01);
  ASSERT_FALSE(p1.warning_mode());
  ASSERT_FALSE(fabs(p2.critical() - 42.0) > 0.00001);
  ASSERT_FALSE(fabs(p2.critical_low() + 456.032) > 0.00001);
  ASSERT_FALSE(p2.critical_mode());
  ASSERT_FALSE(fabs(p2.max() - 76.3) > 0.00001);
  ASSERT_FALSE(fabs(p2.min() - 567.2) > 0.00001);
  ASSERT_FALSE(p2.name() != "foo");
  ASSERT_FALSE(p2.unit() != "bar");
  ASSERT_FALSE(fabs(p2.value() - 52189.912) > 0.00001);
  ASSERT_FALSE(p2.value_type() != storage::perfdata::counter);
  ASSERT_FALSE(fabs(p2.warning() - 4548.0) > 0.00001);
  ASSERT_FALSE(fabs(p2.warning_low() - 42.42) > 0.00001);
  ASSERT_FALSE(!p2.warning_mode());
}

/**
 *  Check that the perfdata copy constructor works properly.
 */
TEST(StoragePerfdata, CopyCtor) {
  // First object.
  storage::perfdata p1;
  p1.critical(42.0);
  p1.critical_low(-456.032);
  p1.critical_mode(false);
  p1.max(76.3);
  p1.min(567.2);
  p1.name("foo");
  p1.unit("bar");
  p1.value(52189.912);
  p1.value_type(storage::perfdata::counter);
  p1.warning(4548.0);
  p1.warning_low(42.42);
  p1.warning_mode(true);

  // Second object.
  storage::perfdata p2(p1);

  // Change first object.
  p1.critical(9432.5);
  p1.critical_low(1000.0001);
  p1.critical_mode(true);
  p1.max(123.0);
  p1.min(843.876);
  p1.name("baz");
  p1.unit("qux");
  p1.value(3485.9);
  p1.value_type(storage::perfdata::derive);
  p1.warning(3612.0);
  p1.warning_low(-987579.0);
  p1.warning_mode(false);

  // Check objects properties values.
  ASSERT_FALSE(fabs(p1.critical() - 9432.5) > 0.00001);
  ASSERT_FALSE(fabs(p1.critical_low() - 1000.0001) > 0.00001);
  ASSERT_FALSE(!p1.critical_mode());
  ASSERT_FALSE(fabs(p1.max() - 123.0) > 0.00001);
  ASSERT_FALSE(fabs(p1.min() - 843.876) > 0.00001);
  ASSERT_FALSE(p1.name() != "baz");
  ASSERT_FALSE(p1.unit() != "qux");
  ASSERT_FALSE(fabs(p1.value() - 3485.9) > 0.00001);
  ASSERT_FALSE(p1.value_type() != storage::perfdata::derive);
  ASSERT_FALSE(fabs(p1.warning() - 3612.0) > 0.00001);
  ASSERT_FALSE(fabs(p1.warning_low() + 987579.0) > 0.01);
  ASSERT_FALSE(p1.warning_mode());
  ASSERT_FALSE(fabs(p2.critical() - 42.0) > 0.00001);
  ASSERT_FALSE(fabs(p2.critical_low() + 456.032) > 0.00001);
  ASSERT_FALSE(p2.critical_mode());
  ASSERT_FALSE(fabs(p2.max() - 76.3) > 0.00001);
  ASSERT_FALSE(fabs(p2.min() - 567.2) > 0.00001);
  ASSERT_FALSE(p2.name() != "foo");
  ASSERT_FALSE(p2.unit() != "bar");
  ASSERT_FALSE(fabs(p2.value() - 52189.912) > 0.00001);
  ASSERT_FALSE(p2.value_type() != storage::perfdata::counter);
  ASSERT_FALSE(fabs(p2.warning() - 4548.0) > 0.00001);
  ASSERT_FALSE(fabs(p2.warning_low() - 42.42) > 0.00001);
  ASSERT_FALSE(!p2.warning_mode());
}

/**
 *  Check that the perfdata object properly default constructs.
 *
 *  @return 0 on success.
 */
TEST(StoragePerfdata, DefaultCtor) {
  // Build object.
  storage::perfdata p;

  // Check properties values.
  ASSERT_FALSE(!std::isnan(p.critical()));
  ASSERT_FALSE(!std::isnan(p.critical_low()));
  ASSERT_FALSE(p.critical_mode());
  ASSERT_FALSE(!std::isnan(p.max()));
  ASSERT_FALSE(!std::isnan(p.min()));
  ASSERT_FALSE(!p.name().empty());
  ASSERT_FALSE(!p.unit().empty());
  ASSERT_FALSE(!std::isnan(p.value()));
  ASSERT_FALSE(p.value_type() != storage::perfdata::gauge);
  ASSERT_FALSE(!std::isnan(p.warning()));
  ASSERT_FALSE(!std::isnan(p.warning_low()));
  ASSERT_FALSE(p.warning_mode());
}

class StorageParserParsePerfdata : public testing::Test {
 public:
  void SetUp() override { config::applier::init(); }
  void TearDown() override { config::applier::deinit(); };
};

// Given a storage::parser object
// When parse_perfdata() is called with a valid perfdata string
// Then perfdata are returned in a list
TEST_F(StorageParserParsePerfdata, Simple1) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  p.parse_perfdata(0, 0, "time=2.45698s;2.000000;5.000000;0.000000;10.000000",
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

TEST_F(StorageParserParsePerfdata, Simple2) {
  // Parse perfdata.
  std::list<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata(0, 0, "'ABCD12E'=18.00%;15:;10:;0;100", list);

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

TEST_F(StorageParserParsePerfdata, Complex1) {
  // Parse perfdata.
  std::list<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata(
      0, 0,
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
TEST_F(StorageParserParsePerfdata, Loop) {
  // Objects.
  std::list<storage::perfdata> list;
  storage::parser p;

  // Loop.
  for (uint32_t i(0); i < 10000; ++i) {
    // Parse perfdata string.
    list.clear();
    p.parse_perfdata(
        0, 0, "c[time]=2.45698s;2.000000;5.000000;0.000000;10.000000", list);

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
TEST_F(StorageParserParsePerfdata, Incorrect1) {
  // Objects.
  std::list<storage::perfdata> list;
  storage::parser p;

  // Attempt to parse perfdata.
  p.parse_perfdata(0, 0, "metric1= 10 metric2=42", list);
  ASSERT_EQ(list.size(), 1);
  ASSERT_EQ(list.back().name(), "metric2");
  ASSERT_EQ(list.back().value(), 42);
}

// Given a storage::parser object
// When parse_perfdata() is called with a metric without value but with unit
// Then it throws a storage::exceptions::perfdata
TEST_F(StorageParserParsePerfdata, Incorrect2) {
  // Given
  std::list<storage::perfdata> list;
  storage::parser p;

  // Then
  p.parse_perfdata(0, 0, "metric=kb/s", list);
  ASSERT_TRUE(list.empty());
}

TEST_F(StorageParserParsePerfdata, LabelWithSpaces) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  p.parse_perfdata(0, 0, "  'foo  bar   '=2s;2;5;;", lst);

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

TEST_F(StorageParserParsePerfdata, LabelWithSpacesMultiline) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  p.parse_perfdata(0, 0, "  'foo  bar   '=2s;2;5;;", lst);

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

TEST_F(StorageParserParsePerfdata, Complex2) {
  // Parse perfdata.
  std::list<storage::perfdata> list;
  storage::parser p;
  p.parse_perfdata(
      0, 0,
      "'  \n time'=2,45698s;;nan;;inf d[metric]=239765B/s;5;;-inf; "
      "g[test]=8x;;;;"
      " infotraffic=18,6x;;;; a[foo]=1234,17;10;11: c[bar]=1234,147;~:10;20:30",
      list);

  // Assertions.
  ASSERT_EQ(list.size(), 6u);
  std::list<storage::perfdata>::const_iterator it(list.begin());
  storage::perfdata expected;

  // #1.
  expected.name("time");
  expected.value_type(storage::perfdata::gauge);
  expected.value(2.45698);
  expected.unit("s");
  expected.max(INFINITY);
  ASSERT_TRUE(expected == *it);
  ASSERT_FALSE(expected != *it);
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
  ASSERT_FALSE(expected != *it);
  ++it;

  // #3.
  expected = storage::perfdata();
  expected.name("test");
  expected.value_type(storage::perfdata::gauge);
  expected.value(8);
  expected.unit("x");
  ASSERT_TRUE(expected == *it);
  ASSERT_FALSE(expected != *it);
  ++it;

  // #4.
  expected = storage::perfdata();
  expected.name("infotraffic");
  expected.value_type(storage::perfdata::gauge);
  expected.value(18.6);
  expected.unit("x");
  ASSERT_TRUE(expected == *it);
  ASSERT_FALSE(expected != *it);
  ++it;

  // #5.
  expected = storage::perfdata();
  expected.name("foo");
  expected.value_type(storage::perfdata::absolute);
  expected.value(1234.17);
  expected.warning(10.0);
  expected.warning_low(0.0);
  expected.critical(INFINITY);
  expected.critical_low(11.0);
  ASSERT_TRUE(expected == *it);
  ASSERT_FALSE(expected != *it);
  ++it;

  // #6.
  expected = storage::perfdata();
  expected.name("bar");
  expected.value_type(storage::perfdata::counter);
  expected.value(1234.147);
  expected.warning(10.0);
  expected.warning_low(-INFINITY);
  expected.critical(30.0);
  expected.critical_low(20.0);
  ASSERT_TRUE(expected == *it);
  ASSERT_FALSE(expected != *it);
  ++it;
}

// Given a storage::parser object
// When parse_perfdata() is called with a valid perfdata string
// Then perfdata are returned in a list
TEST_F(StorageParserParsePerfdata, SimpleWithR) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  // ASSERT_NO_THROW(p.parse_perfdata("'total'=5;;;0;\r", lst));
  ASSERT_NO_THROW(p.parse_perfdata(0, 0, "'total'=5;;;0;\r", lst));

  // Assertions.
  ASSERT_EQ(lst.size(), 1u);
  std::list<storage::perfdata>::const_iterator it(lst.begin());
  storage::perfdata expected;
  expected.name("total");
  expected.value_type(storage::perfdata::gauge);
  expected.value(5);
  expected.unit("");
  expected.warning(NAN);
  expected.warning_low(NAN);
  expected.critical(NAN);
  expected.critical_low(NAN);
  expected.min(0.0);
  expected.max(NAN);
  ASSERT_TRUE(expected == *it);
}

// Given a storage::parser object
// When parse_perfdata() is called with a valid perfdata string
// Then perfdata are returned in a list
TEST_F(StorageParserParsePerfdata, BadMetric) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  ASSERT_NO_THROW(p.parse_perfdata(0, 0, "user1=1 user2=2 =1 user3=3", lst));

  // Assertions.
  ASSERT_EQ(lst.size(), 3u);
  int i = 1;
  for (auto& p : lst) {
    ASSERT_EQ(p.name(), fmt::format("user{}", i));
    ASSERT_EQ(p.value(), static_cast<double>(i));
    ++i;
  }
}

TEST_F(StorageParserParsePerfdata, BadMetric1) {
  // Parse perfdata.
  std::list<storage::perfdata> lst;
  storage::parser p;
  ASSERT_NO_THROW(
      p.parse_perfdata(0, 0, "user1=1 user2=2 user4= user3=3", lst));

  // Assertions.
  ASSERT_EQ(lst.size(), 3u);
  int i = 1;
  for (auto& p : lst) {
    ASSERT_EQ(p.name(), fmt::format("user{}", i));
    ASSERT_EQ(p.value(), static_cast<double>(i));
    ++i;
  }
}
