/*
** Copyright 2012-2013 Centreon
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

#include <cmath>
#include <cstdlib>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/event_handler.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check event_handler's default constructor.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Object.
  neb::event_handler evnt_hndlr;

  // Check.
  return (((evnt_hndlr.source_id != 0)
           || (evnt_hndlr.destination_id != 0)
           || (evnt_hndlr.command_args != "")
           || (evnt_hndlr.command_line != "")
           || (evnt_hndlr.early_timeout != 0)
           || (evnt_hndlr.end_time != 0)
           || (fabs(evnt_hndlr.execution_time) > 0.000001)
           || (evnt_hndlr.handler_type != 0)
           || (evnt_hndlr.host_id != 0)
           || (evnt_hndlr.output != "")
           || (evnt_hndlr.return_code != 0)
           || (evnt_hndlr.service_id != 0)
           || (evnt_hndlr.start_time != 0)
           || (evnt_hndlr.state != 0)
           || (evnt_hndlr.state_type != 0)
           || (evnt_hndlr.timeout != 0)
           || (evnt_hndlr.type()
               != io::events::data_type<
                                io::events::neb,
                                neb::de_event_handler>::value))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
