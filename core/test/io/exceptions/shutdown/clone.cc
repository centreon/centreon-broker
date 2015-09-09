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
#include <memory>
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker;

/**
 *  Check that the exception cloning works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  io::exceptions::shutdown s(true, false);
  s << "foo" << 42 << 77454654249841ull << -1 << "bar";

  // Clone object.
  std::auto_ptr<exceptions::msg> clone(s.clone());

  // Check that clone was properly constructed.
  int retval(!clone.get());

  // Check that this is really a shutdown exception object.
  if (!retval) {
    try {
      clone->rethrow();
    }
    catch (io::exceptions::shutdown const& s) {
      retval |= (
        strcmp("foo4277454654249841-1bar", s.what())
        || !s.is_in_shutdown()
        || s.is_out_shutdown());
    }
    catch (...) {
      retval |= 1;
    }
  }

  // Return check result.
  return (retval);
}
