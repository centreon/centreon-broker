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

#include "com/centreon/broker/storage/rebuild.hh"
#include <gtest/gtest.h>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check that the rebuild object properly default constructs.
 */
TEST(StorageRebuild, DefaultCtor) {
  // Build object.
  storage::rebuild r;

  // Check properties values.
  ASSERT_FALSE(!r.end);
  ASSERT_FALSE(r.id != 0);
  ASSERT_FALSE(r.is_index);
}

/**
 *  Check that the remove_graph object properly return is type
 */
TEST(StorageRebuild, ReturnType) {
  storage::rebuild r;
  auto val =
      io::events::data_type<io::events::storage, storage::de_rebuild>::value;

  ASSERT_TRUE(r.static_type() == val);
  ASSERT_TRUE(r.type() == val);
}
