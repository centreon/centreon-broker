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

#include "com/centreon/broker/storage/status.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check that the status assignment operator works properly.
 */
TEST(StorageStatus, Assign) {
  // First object.
  storage::status s1;
  s1.ctime = 123456789;
  s1.index_id = 6774;
  s1.interval = 42;
  s1.is_for_rebuild = true;
  s1.rrd_len = 180;
  s1.state = 3;

  // Second object.
  storage::status s2;
  s2.ctime = 654123;
  s2.index_id = 33;
  s2.interval = 78;
  s2.is_for_rebuild = false;
  s2.rrd_len = 666;
  s2.state = 1;

  // Assignment.
  s2 = s1;

  // Change first object.
  s1.ctime = 741258;
  s1.index_id = 12;
  s1.interval = 36;
  s1.is_for_rebuild = false;
  s1.rrd_len = 900;
  s1.state = 2;

  // Check objects properties values.
  ASSERT_FALSE(s1.ctime != 741258);
  ASSERT_FALSE(s1.index_id != 12);
  ASSERT_FALSE(s1.interval != 36);
  ASSERT_FALSE(s1.is_for_rebuild != false);
  ASSERT_FALSE(s1.rrd_len != 900);
  ASSERT_FALSE(s1.state != 2);
  ASSERT_FALSE(s2.ctime != 123456789);
  ASSERT_FALSE(s2.index_id != 6774);
  ASSERT_FALSE(s2.interval != 42);
  ASSERT_FALSE(s2.is_for_rebuild != true);
  ASSERT_FALSE(s2.rrd_len != 180);
  ASSERT_FALSE(s2.state != 3);
}

/**
 *  Check that the status copy constructor works properly.
 */
TEST(StorageStatus, CopyCtor) {
  // First object.
  storage::status s1;
  s1.ctime = 123456789;
  s1.index_id = 42;
  s1.interval = 24;
  s1.is_for_rebuild = true;
  s1.rrd_len = 180;
  s1.state = 1;

  // Second object.
  storage::status s2(s1);

  // Change first object.
  s1.ctime = 741258;
  s1.index_id = 3612;
  s1.interval = 36;
  s1.is_for_rebuild = false;
  s1.rrd_len = 900;
  s1.state = 2;

  // Check objects properties values.
  ASSERT_FALSE(s1.ctime != 741258);
  ASSERT_FALSE(s1.index_id != 3612);
  ASSERT_FALSE(s1.interval != 36);
  ASSERT_FALSE(s1.is_for_rebuild != false);
  ASSERT_FALSE(s1.rrd_len != 900);
  ASSERT_FALSE(s1.state != 2);
  ASSERT_FALSE(s2.ctime != 123456789);
  ASSERT_FALSE(s2.index_id != 42);
  ASSERT_FALSE(s2.interval != 24);
  ASSERT_FALSE(s2.is_for_rebuild != true);
  ASSERT_FALSE(s2.rrd_len != 180);
  ASSERT_FALSE(s2.state != 1);
  ;
}

/**
 *  Check that the status object properly default constructs.
 */
TEST(StorageStatus, DefaultCtor) {
  // Build object.
  storage::status s;

  auto val(
      io::events::data_type<io::events::storage, storage::de_status>::value);

  // Check properties values.
  ASSERT_FALSE(s.ctime != 0);
  ASSERT_FALSE(s.index_id != 0);
  ASSERT_FALSE(s.interval != 0);
  ASSERT_FALSE(s.is_for_rebuild != false);
  ASSERT_FALSE(s.rrd_len != 0);
  ASSERT_FALSE(s.state != 0);
  ASSERT_FALSE(s.type() != val);
  ;
}
