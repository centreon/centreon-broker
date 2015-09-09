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

#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that a initial host state log is properly parsed.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  int retval(EXIT_FAILURE);

  try {
    // #1
    {
      neb::log_entry le;
      neb::set_log_data(
        le,
        "INITIAL HOST STATE: myserver;UP;HARD;1;PING OK");
      if ((le.host_name != "myserver")
          || (le.log_type != 1) // HARD
          || (le.msg_type != 9) // INITIAL HOST STATE
          || (le.output != "PING OK")
          || (le.retry != 1)
          || (le.status != 0)) // UP
        throw (exceptions::msg() << "log entry #1 mismatch");
    }

    // #2
    {
      neb::log_entry le;
      neb::set_log_data(
        le,
        "INITIAL HOST STATE: SERVER007;UNKNOWN;SOFT;2;");
      if ((le.host_name != "SERVER007")
          || (le.log_type != 0) // SOFT
          || (le.msg_type != 9) // INITIAL HOST STATE
          || (le.output != "")
          || (le.retry != 2)
          || (le.status != 3)) // UNKNOWN
        throw (exceptions::msg() << "log entry #2 mismatch");
    }

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  return (retval);
}
