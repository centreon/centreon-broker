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

#ifndef EVENTS_PROGRAM_H_
# define EVENTS_PROGRAM_H_

# include <time.h>          // for time_t
# include <string>
# include "events/event.h"

namespace       Events
{
  /**
   *  \class Program program.h "events/program.h"
   *  \brief Information about Nagios process.
   *
   *  Program holds information about a Nagios process, like whether it is
   *  running or not, in daemon mode or not, ...
   */
  class         Program : public Event
  {
   private:
    void        InternalCopy(const Program& program);

   public:
    bool        daemon_mode;
    int         instance_id;
    std::string instance_name;
    bool        is_running;
    int         pid;
    time_t      program_end;
    time_t      program_start;
                Program();
                Program(const Program& program);
                ~Program();
    Program&    operator=(const Program& program);
    int         GetType() const;
  };
}

#endif /* !EVENTS_PROGRAM_H_ */
