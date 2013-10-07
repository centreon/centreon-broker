/*
** Copyright 2012-2013 Merethis
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_dependency.hh"

using namespace com::centreon::broker;

/**
 *  Check service_dependency's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::service_dependency sdep;

  // Check.
  return ((sdep.dependency_period != "")
          || (sdep.dependent_host_id != 0)
          || (sdep.enabled != true)
          || (sdep.execution_failure_options != "")
          || (sdep.host_id != 0)
          || (sdep.inherits_parent != false)
          || (sdep.notification_failure_options != "")
          || (sdep.service_id != 0)
          || (sdep.type()
              != io::events::data_type<io::events::neb, neb::de_service_dependency>::value));
}
