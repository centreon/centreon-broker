/*
 * Copyright 2019 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/influxdb/column.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

TEST(InfluxDbColumn, Simple) {
  influxdb::column col("host", "test", true, influxdb::column::string);

  ASSERT_EQ(col.get_name(), "host");
  ASSERT_EQ(col.get_value(), "test");
  ASSERT_EQ(col.get_type(), influxdb::column::string);
  ASSERT_EQ(col.is_flag(), true);
}

TEST(InfluxDbColumn, DefaultCtor) {
  influxdb::column col;

  ASSERT_EQ(col.get_name(), "");
  ASSERT_EQ(col.get_value(), "");
  ASSERT_EQ(col.get_type(), influxdb::column::number);
  ASSERT_EQ(col.is_flag(), false);
}

TEST(InfluxDbColumn, CopyCtor) {
  influxdb::column col("host", "test", true, influxdb::column::string);
  influxdb::column col2{col};

  ASSERT_EQ(col2.get_name(), "host");
  ASSERT_EQ(col2.get_value(), "test");
  ASSERT_EQ(col2.get_type(), influxdb::column::string);
  ASSERT_EQ(col2.is_flag(), true);
}

TEST(InfluxDbColumn, Assign) {
  influxdb::column col("host", "test", true, influxdb::column::string);
  influxdb::column col2;

  col2 = col;

  ASSERT_EQ(col2.get_name(), "host");
  ASSERT_EQ(col2.get_value(), "test");
  ASSERT_EQ(col2.get_type(), influxdb::column::string);
  ASSERT_EQ(col2.is_flag(), true);
}

TEST(InfluxDbColumn, ParseType) {
  ASSERT_EQ(influxdb::column::parse_type("string"), influxdb::column::string);
  ASSERT_EQ(influxdb::column::parse_type("number"), influxdb::column::number);
  ASSERT_THROW(influxdb::column::parse_type("other"), exceptions::msg);
}