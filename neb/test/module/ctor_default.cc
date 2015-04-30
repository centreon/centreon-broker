/*
** Copyright 2012-2013,2015 Merethis
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
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/module.hh"

using namespace com::centreon::broker;

/**
 *  Check module's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::module mod;

  // Check.
  return (((mod.source_id != 0)
           || (mod.destination_id != 0)
           || (mod.args != "")
           || (mod.enabled != true)
           || (mod.filename != "")
           || (mod.loaded != false)
           || (mod.should_be_loaded != false)
           || (mod.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_module>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
