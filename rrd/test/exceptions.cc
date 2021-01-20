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

#include <memory>

#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(RrdExceptions, OpenThrow) {
  // Return value.
  int retval(0);

  // Second throw.
  try {
    try {
      throw rrd::exceptions::open("{}{}{}{}", "baz", 42, "qux", -789410l);
      retval |= 1;
    } catch (msg_fmt const& e) {
      retval |= strcmp(e.what(), "baz42qux-789410");
    }
  } catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw rrd::exceptions::open("{}{}{}", "foobarbazqux", -74125896321445ll,
                                  36);
      retval |= 1;
    } catch (std::exception const& e) {
      retval |= strcmp(e.what(), "foobarbazqux-7412589632144536");
    }
  } catch (...) {
    retval |= 1;
  }

  // Return test result.
  ASSERT_TRUE(0 == retval);
}

TEST(RrdExceptions, UpdateThrow) {
  // Return value.
  int retval(0);

  // First throw.
  try {
    try {
      throw rrd::exceptions::update("{}{}{}", "foobar", 42, -789654ll);
      retval |= 1;
    } catch (rrd::exceptions::update const& e) {
      retval |= strcmp(e.what(), "foobar42-789654");
    }
  } catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw rrd::exceptions::update("{}{}{}{}", "baz", 42, "qux", -789410l);
      retval |= 1;
    } catch (msg_fmt const& e) {
      retval |= strcmp(e.what(), "baz42qux-789410");
    }
  } catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw rrd::exceptions::update("{}{}{}", "foobarbazqux", -74125896321445ll,
                                    36);
      retval |= 1;
    } catch (std::exception const& e) {
      retval |= strcmp(e.what(), "foobarbazqux-7412589632144536");
    }
  } catch (...) {
    retval |= 1;
  }

  // Return test result.
  ASSERT_TRUE(0 == retval);
}
