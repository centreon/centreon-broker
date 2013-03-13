/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
