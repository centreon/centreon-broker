/*
** Copyright 2011-2012 Centreon
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

#include <cstdlib>
#include <gtest/gtest.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;

/**
 *  We should not be able to read from publisher.
 *
 */
TEST(Publisher, Read) {
  // Initialization.
  config::applier::init();

  // Error flag.
  bool error(true);

  // Publisher.
  multiplexing::publisher p;

  // Read from publisher.
  try {
    std::shared_ptr<io::data> d;
    p.read(d);
    error = true;
  }
  catch (std::exception const& e) {
    error = false;
  }
  catch (...) {
    error = true;
  }

  // Cleanup.
  config::applier::deinit();

  // Return.
  ASSERT_FALSE(error);
}
