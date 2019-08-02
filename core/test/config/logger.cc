/*
 * Copyright 2005 - 2019 Centreon (https://www.centreon.com/)
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
#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that the logger configuration class can be copied properly.
 *
 *  @return 0 on success.
 */
TEST(logger, assignment) {
  // First object.
  config::logger l1;
  l1.config(false);
  l1.debug(true);
  l1.error(true);
  l1.info(false);
  l1.level(logging::medium);
  l1.max_size(1456354874ull);
  l1.name("FooBar");
  l1.type(config::logger::file);

  // Second object.
  config::logger l2;
  l2.config(false);
  l2.debug(true);
  l2.error(true);
  l2.info(true);
  l2.level(logging::none);
  l2.max_size(8485798487ull);
  l2.name("BazQux");
  l2.type(config::logger::syslog);

  // Assignment.
  l2 = l1;

  // Change first object.
  l1.config(true);
  l1.debug(false);
  l1.error(false);
  l1.info(true);
  l1.level(logging::low);
  l1.max_size(484587188ull);
  l1.name("BazQux");
  l1.type(config::logger::standard);

  // Check.
  ASSERT_FALSE(!l1.config());
  ASSERT_FALSE(l1.debug());
  ASSERT_FALSE(l1.error());
  ASSERT_FALSE(!l1.info());
  ASSERT_FALSE(l1.level() != logging::low);
  ASSERT_FALSE(l1.max_size() != 484587188ull);
  ASSERT_FALSE(l1.name() != "BazQux");
  ASSERT_FALSE(l1.type() != config::logger::standard);
  ASSERT_FALSE(l2.config());
  ASSERT_FALSE(!l2.debug());
  ASSERT_FALSE(!l2.error());
  ASSERT_FALSE(l2.info());
  ASSERT_FALSE(l2.level() != logging::medium);
  ASSERT_FALSE(l2.max_size() != 1456354874ull);
  ASSERT_FALSE(l2.name() != "FooBar");
  ASSERT_FALSE(l2.type() != config::logger::file);
}

TEST(logger, copyCtor) {
  // First object.
  config::logger l1;
  l1.config(false);
  l1.debug(true);
  l1.error(true);
  l1.info(false);
  l1.level(logging::medium);
  l1.max_size(1456354874ull);
  l1.name("FooBar");
  l1.type(config::logger::file);

  // Second object.
  config::logger l2(l1);

  // Change first object.
  l1.config(true);
  l1.debug(false);
  l1.error(false);
  l1.info(true);
  l1.level(logging::low);
  l1.max_size(484587188ull);
  l1.name("BazQux");
  l1.type(config::logger::standard);

  // Check.
  ASSERT_FALSE(!l1.config());
  ASSERT_FALSE(l1.debug());
  ASSERT_FALSE(l1.error());
  ASSERT_FALSE(!l1.info());
  ASSERT_FALSE(l1.level() != logging::low);
  ASSERT_FALSE(l1.max_size() != 484587188ull);
  ASSERT_FALSE(l1.name() != "BazQux");
  ASSERT_FALSE(l1.type() != config::logger::standard);
  ASSERT_FALSE(l2.config());
  ASSERT_FALSE(!l2.debug());
  ASSERT_FALSE(!l2.error());
  ASSERT_FALSE(l2.info());
  ASSERT_FALSE(l2.level() != logging::medium);
  ASSERT_FALSE(l2.max_size() != 1456354874ull);
  ASSERT_FALSE(l2.name() != "FooBar");
  ASSERT_FALSE(l2.type() != config::logger::file);
}

TEST(logger, defaultCtor) {
  // Logger configuration.
  config::logger cfg;

  // Check content.
  ASSERT_FALSE(!cfg.config());
  ASSERT_FALSE(cfg.debug());
  ASSERT_FALSE(!cfg.error());
  ASSERT_FALSE(cfg.info());
  ASSERT_FALSE(cfg.level() != logging::high);
  ASSERT_FALSE(cfg.max_size() != 10000000000ull);
  ASSERT_FALSE(!cfg.name().isEmpty());
  ASSERT_FALSE(cfg.type() != config::logger::unknown);
}

TEST(logger, equality) {
  // First object.
  config::logger l1;
  l1.config(true);
  l1.debug(true);
  l1.error(true);
  l1.info(true);
  l1.level(logging::high);
  l1.max_size(98784818758745ull);
  l1.name("FooBar");
  l1.type(config::logger::standard);

  // Second object.
  config::logger l2(l1);

  // Third object.
  config::logger l3(l1);
  l3.config(false);

  // Check
  ASSERT_FALSE(!(l1 == l2) || (l2 == l3));
}

TEST(logger, inequality) {
  // First object.
  config::logger l1;
  l1.config(true);
  l1.debug(true);
  l1.error(true);
  l1.info(true);
  l1.level(logging::high);
  l1.max_size(7685165485157ull);
  l1.name("FooBar");
  l1.type(config::logger::standard);

  // Second object.
  config::logger l2(l1);

  // Third object.
  config::logger l3(l1);
  l3.config(false);

  // Check
  ASSERT_FALSE((l1 != l2) || !(l2 != l3));
}
