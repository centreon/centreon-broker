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
 *  Chat that assignment operator works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  io::exceptions::shutdown s1(false, true);
  s1 << 4189545612ul << "foo   " << " bar" << -123456789ll;

  // Second object.
  io::exceptions::shutdown s2(false, false);
  s2 << "baz" << 42u << 123456 << -7410;

  // Assign.
  s2 = s1;

  // Update first object.
  s1 << "qux";

  // Check.
  return (strcmp(s1.what(), "4189545612foo    bar-123456789qux")
	  || (strcmp(s2.what(), "4189545612foo    bar-123456789"))
	  || s1.is_in_shutdown()
	  || !s1.is_out_shutdown()
	  || s2.is_in_shutdown()
	  || !s2.is_out_shutdown());
}
