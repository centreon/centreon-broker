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

#include <cstring>
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

/**
 *  Check that the exception cloning works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  exceptions::msg e;
  e << "foo" << 42 << 77454654249841ull << -1 << "bar";

  // Clone object.
  std::unique_ptr<exceptions::msg> clone(e.clone());

  // Check that clone was properly constructed.
  int retval(!clone.get()
             || strcmp("foo4277454654249841-1bar", clone->what()));

  // Check that this is really a msg exception object.
  if (!retval) {
    try {
      clone->rethrow();
    }
    catch (exceptions::msg const& e) {
      (void)e; // We're good.
    }
    catch (...) {
      retval |= 1;
    }
  }

  // Return check result.
  return (retval);
}
