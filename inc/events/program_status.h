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

# include <ctime> // for time_t
# include <string>
# include "events/status.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                ProgramStatus : public Status
    {
     private:
      void               InternalCopy(const ProgramStatus& ps);

     public:
      bool               active_host_checks_enabled;
      bool               active_service_checks_enabled;
      bool               daemon_mode;
      std::string        global_host_event_handler;
      std::string        global_service_event_handler;
      bool               is_running;
      time_t             last_alive;
      time_t             last_command_check;
      time_t             last_log_rotation;
      int                modified_host_attributes;
      int                modified_service_attributes;
      bool               obsess_over_hosts;
      bool               obsess_over_services;
      bool               passive_host_checks_enabled;
      bool               passive_service_checks_enabled;
      int                pid;
      time_t             program_end;
      time_t             program_start;
                         ProgramStatus();
			 ProgramStatus(const ProgramStatus& ps);
			 ~ProgramStatus();
      ProgramStatus&     operator=(const ProgramStatus& ps);
      int                GetType() const throw ();
    };
  }
}

#endif /* !EVENTS_PROGRAM_STATUS_H_ */
