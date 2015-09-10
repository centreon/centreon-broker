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
#include "com/centreon/broker/storage/exceptions/perfdata.hh"

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
      throw (storage::exceptions::perfdata()
               << "foobar" << 42 << -789654ll);
      retval |= 1;
    }
    catch (storage::exceptions::perfdata const& e) {
      retval |= strcmp(e.what(), "foobar42-789654");
    }
  }
  catch (...) {
    retval |= 1;
  }

  // Second throw.
  try {
    try {
      throw (storage::exceptions::perfdata()
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
      throw (storage::exceptions::perfdata()
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
  return (retval);
}
