/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef EVENTS_PROGRAM_STATUS_H_
# define EVENTS_PROGRAM_STATUS_H_

# include <time.h>          // for time_t
# include <string>
# include "events/status.h"

namespace              Events
{
  /**
   *  \class ProgramStatus program_status.h "events/program_status.h"
   *  \brief Information about Nagios process.
   *
   *  ProgramStatus holds information about a Nagios process, like whether it
   *  is running or not, in daemon mode or not, ...
   */
  class                ProgramStatus : public Status
  {
   private:
    void               InternalCopy(const ProgramStatus& ps);

   public:
    bool               active_host_checks_enabled;
    bool               active_service_checks_enabled;
    std::string        global_host_event_handler;
    std::string        global_service_event_handler;
    time_t             last_alive;
    time_t             last_command_check;
    time_t             last_log_rotation;
    int                modified_host_attributes;
    int                modified_service_attributes;
    bool               obsess_over_hosts;
    bool               obsess_over_services;
    bool               passive_host_checks_enabled;
    bool               passive_service_checks_enabled;
                       ProgramStatus();
                       ProgramStatus(const ProgramStatus& ps);
                       ~ProgramStatus();
    ProgramStatus&     operator=(const ProgramStatus& ps);
    int                GetType() const;
  };
}

#endif /* !EVENTS_PROGRAM_STATUS_H_ */
