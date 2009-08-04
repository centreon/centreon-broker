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

#ifndef EVENTS_LOG_H_
# define EVENTS_LOG_H_

# include <ctime> // for time_t
# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                Log : public Event
    {
     private:
      void               InternalCopy(const Log& log);

     public:
      time_t             c_time;
      std::string        host;
      int                msg_type;
      std::string        notification_cmd;
      std::string        notification_contact;
      std::string        output;
      int                retry;
      std::string        service;
      std::string        status;
      std::string        type;
                         Log();
                         Log(const Log& log);
                         ~Log();
      Log&               operator=(const Log& log);
      int                GetType() const throw ();
    };
  }
}

#endif /* !EVENTS_LOG_H_ */
