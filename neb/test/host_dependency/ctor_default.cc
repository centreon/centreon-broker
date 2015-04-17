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
#include "com/centreon/broker/neb/host_dependency.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check host_dependency's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::host_dependency hdep;

  // Check.
  return (((hdep.source_id != 0)
           || (hdep.destination_id != 0)
           || (hdep.dependency_period != "")
           || (hdep.dependent_host_id != 0)
           || (hdep.enabled != true)
           || (hdep.execution_failure_options != "")
           || (hdep.host_id != 0)
           || (hdep.inherits_parent != false)
           || (hdep.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_host_dependency>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
