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

#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/event_handler.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check event_handler's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::event_handler evnt_hndlr;

  // Check.
  return ((evnt_hndlr.command_args != "")
          || (evnt_hndlr.command_line != "")
          || (evnt_hndlr.early_timeout != 0)
          || (evnt_hndlr.end_time != 0)
          || (fabs(evnt_hndlr.execution_time) > 0.000001)
          || (evnt_hndlr.handler_type != 0)
          || (evnt_hndlr.host_id != 0)
          || (evnt_hndlr.instance_id != 0)
          || (evnt_hndlr.output != "")
          || (evnt_hndlr.return_code != 0)
          || (evnt_hndlr.service_id != 0)
          || (evnt_hndlr.start_time != 0)
          || (evnt_hndlr.state != 0)
          || (evnt_hndlr.state_type != 0)
          || (evnt_hndlr.timeout != 0)
          || (evnt_hndlr.type()
              != io::events::data_type<io::events::neb, neb::de_event_handler>::value));
}
