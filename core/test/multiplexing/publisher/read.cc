/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;

/**
 *  We should not be able to read from publisher.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Return value.
  int retval(0);

  // Publisher.
  multiplexing::publisher p;

  // Read from publisher.
  try {
    misc::shared_ptr<io::data> d;
    p.read(d);
    retval = 1;
  }
  catch (...) {}

  // Cleanup.
  config::applier::deinit();

  // Return.
  return (retval);
}
