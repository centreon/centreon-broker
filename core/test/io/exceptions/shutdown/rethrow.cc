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
 *  Check that the rethrow() interface works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  try {
    try {
      // Initial throw.
      throw (io::exceptions::shutdown(false, true)
               << "foobar" << 42 << -789654123ll);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& s) { // Proper catch.
      s.rethrow();
    }
  }
  catch (io::exceptions::shutdown const& s) { // Catch rethrow.
    retval = (strcmp(s.what(), "foobar42-789654123")
              || s.is_in_shutdown()
              || !s.is_out_shutdown());
  }
  catch (...) {
    retval |= 1;
  }

  // Return check result.
  return (retval);
}
