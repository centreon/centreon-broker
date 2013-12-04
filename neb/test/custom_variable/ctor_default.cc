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
#include "com/centreon/broker/neb/custom_variable.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check custom_variable's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::custom_variable cvar;

  // Check.
  return ((cvar.host_id != 0)
          || (cvar.modified != false)
          || (cvar.name != "")
          || (cvar.service_id != 0)
          || (cvar.update_time != 0)
          || (cvar.value != "")
          || (cvar.var_type != 0)
          || (cvar.type()
              != io::events::data_type<io::events::neb, neb::de_custom_variable>::value));
}
