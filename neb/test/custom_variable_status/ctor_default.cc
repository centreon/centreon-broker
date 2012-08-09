/*
** Copyright 2012 Merethis
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

#include "com/centreon/broker/neb/custom_variable_status.hh"

using namespace com::centreon::broker;

/**
 *  Check custom_variable_status's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::custom_variable_status cvar_status;

  // Check.
  return ((cvar_status.host_id != 0)
          || (cvar_status.modified != true)
          || (cvar_status.name != "")
          || (cvar_status.service_id != 0)
          || (cvar_status.update_time != 0)
          || (cvar_status.value != "")
          || (cvar_status.type()
              != "com::centreon::broker::neb::custom_variable_status"));
}
