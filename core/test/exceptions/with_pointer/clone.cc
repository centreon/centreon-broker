/*
** Copyright 2013 Centreon
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
#include <cstring>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

/**
 *  Check that the exception cloning works properly.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Base exception.
  exceptions::msg base;
  base << "foo" << 42 << 77454654249841ull << -1 << "bar";
  misc::shared_ptr<io::data> dat(new io::raw);
  exceptions::with_pointer e(base, dat);

  // Clone object.
  std::auto_ptr<exceptions::msg> clone(e.clone());

  // Check that clone was properly constructed.
  bool error(!clone.get() || strcmp(e.what(), base.what()));

  // The base exception must be thrown.
  if (!error) {
    try {
      clone->rethrow();
    }
    catch (exceptions::msg const& e) {
      // Proper type catch, check content.
      error = (error || strcmp(e.what(), base.what()));
    }
    catch (...) {
      error = true;
    }
  }

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
