/*
** Copyright 2011-2013 Centreon
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
#include <cstdlib>
#include <ctime>
#include <memory>
#include <unistd.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/rrd/lib.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"

using namespace com::centreon::broker;

/**
 *  Check that the exception cloning works properly.
 *
 *  @return 0 on success.
 */
TEST(RrdExceptions, OpenClone) {
  // Base object.
  rrd::exceptions::open e;
  e << "foo" << 42 << 77454654249841ull << -1 << "bar";

  // Clone object.
  std::unique_ptr<exceptions::msg> clone(e.clone());

  // Check that clone object was properly constructed.
  int retval(!clone.get()
             || strcmp("foo4277454654249841-1bar", clone->what()));

  // Check that this is really an open error.
  if (!retval) {
    try {
      static_cast<rrd::exceptions::open *>(clone.get())->rethrow();
    }
    catch (rrd::exceptions::open const& e) {
      (void)e; // We're good.
    }
    catch (...) {
      retval |= 1;
    }
  }

  // Return check result.
  ASSERT_TRUE(0 == retval);
}

TEST(RrdExceptions, OpenRethrow) {
  // Return value.
  int retval(0);

  try {
    try {
      // Initial throw.
      throw (rrd::exceptions::open()
        << "foobar" << 42 << -789654123ll);
      retval |= 1;
    }
    catch (exceptions::msg const& e) { // Proper catch.
      //e.rethrow();
    }
  }
  catch (rrd::exceptions::open const& e) { // Catch rethrown exception.
    retval |= strcmp(e.what(), "foobar42-789654123");
  }
  catch (...) {
    retval |= 1;
  }

  // Return check result.
  ASSERT_TRUE(0 == retval);
}

TEST(RrdExceptions, OpenThrow) {
  // Return value.
  int retval(0);

  // First throw.
  try {
    try {
      throw (rrd::exceptions::open()
        << "foobar" << 42 << -789654ll);
      retval |= 1;
    }
    catch (rrd::exceptions::open const& e) {
      retval |= strcmp(e.what(), "foobar42-789654");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw (rrd::exceptions::open()
        << "baz" << 42 << "qux" << -789410l);
      retval |= 1;
    }
    catch (exceptions::msg const& e) {
      retval |= strcmp(e.what(), "baz42qux-789410");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw (rrd::exceptions::open()
        << "foobarbazqux" << -74125896321445ll << 36);
      retval |= 1;
    }
    catch (std::exception const& e) {
      retval |= strcmp(e.what(), "foobarbazqux-7412589632144536");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Return test result.
  ASSERT_TRUE(0 == retval);
}

/**
 *  Check that the exception cloning works properly.
 *
 *  @return 0 on success.
 */
TEST(RrdExceptions, UpdateClone) {
  // Base object.
  rrd::exceptions::update e;
  e << "foo" << 42 << 77454654249841ull << -1 << "bar";

  // Clone object.
  std::unique_ptr<exceptions::msg> clone(e.clone());

  // Check that clone object was properly constructed.
  int retval(!clone.get()
             || strcmp("foo4277454654249841-1bar", clone->what()));

  // Check that this is really an open error.
  if (!retval) {
    try {
      static_cast<rrd::exceptions::update *>(clone.get())->rethrow();
    }
    catch (rrd::exceptions::update const& e) {
      (void)e; // We're good.
    }

    catch (...) {
      retval |= 1;
    }
  }

  // Return check result.
  ASSERT_TRUE(0 == retval);
}

TEST(RrdExceptions, UpdateRethrow) {
  // Return value.
  int retval(0);

  try {
    try {
      // Initial throw.
      throw (rrd::exceptions::update()
        << "foobar" << 42 << -789654123ll);
      retval |= 1;
    }
    catch (exceptions::msg const& e) { // Proper catch.
      //e.rethrow();
    }
  }
  catch (rrd::exceptions::update const& e) { // Catch rethrown exception.
    retval |= strcmp(e.what(), "foobar42-789654123");
  }
  catch (...) {
    retval |= 1;
  }

  // Return check result.
  ASSERT_TRUE(0 == retval);
}

TEST(RrdExceptions, UpdateThrow) {
  // Return value.
  int retval(0);

  // First throw.
  try {
    try {
      throw (rrd::exceptions::update()
        << "foobar" << 42 << -789654ll);
      retval |= 1;
    }
    catch (rrd::exceptions::update const& e) {
      retval |= strcmp(e.what(), "foobar42-789654");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw (rrd::exceptions::update()
        << "baz" << 42 << "qux" << -789410l);
      retval |= 1;
    }
    catch (exceptions::msg const& e) {
      retval |= strcmp(e.what(), "baz42qux-789410");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw (rrd::exceptions::update()
        << "foobarbazqux" << -74125896321445ll << 36);
      retval |= 1;
    }
    catch (std::exception const& e) {
      retval |= strcmp(e.what(), "foobarbazqux-7412589632144536");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Return test result.
  ASSERT_TRUE(0 == retval);
}
