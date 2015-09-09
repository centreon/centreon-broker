/*
** Copyright 2011 Centreon
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

#include <cstring>
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker;

/**
 *  Check that exception is properly thrown.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // First throw.
  try {
    try {
      throw (io::exceptions::shutdown(true, false)
               << "foobar" << 42 << -789654ll);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& s) { // Properly caught.
      retval |= (strcmp(s.what(), "foobar42-789654")
                 || !s.is_in_shutdown()
                 || s.is_out_shutdown());
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw (io::exceptions::shutdown(false, false)
               << -36 << "fooqux" << true);
      retval |= 1;
    }
    catch (exceptions::msg const& m) {
      retval |= strcmp(m.what(), "-36fooquxtrue");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Third throw.
  try {
    try {
      throw (io::exceptions::shutdown(true, true)
               << "bazqux" << -74125896321445ll << 36);
      retval |= 1;
    }
    catch (std::exception const& e) {
      retval |= strcmp(e.what(), "bazqux-7412589632144536");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Return test result.
  return (retval);
}
