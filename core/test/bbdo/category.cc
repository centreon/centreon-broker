/*
 * Copyright 2021 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;

/**
 * @brief This test is very important, in case of a change in a category,
 *        BBDO is no more compatible with previous versions.
 *
 * @param CategoryTest
 * @param BbdoCategory
 */
TEST(CategoryTest, BbdoCategory) {
  ASSERT_EQ(io::events::neb, 1);
  ASSERT_EQ(io::events::bbdo, 2);
  ASSERT_EQ(io::events::storage, 3);
  ASSERT_EQ(io::events::dumper, 5);
  ASSERT_EQ(io::events::bam, 6);
  ASSERT_EQ(io::events::extcmd, 7);
  ASSERT_EQ(io::events::generator, 8);
  ASSERT_EQ(io::events::internal, 65535);
}
