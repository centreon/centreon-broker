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

#ifndef EVENTS_DOWNTIME_H_
# define EVENTS_DOWNTIME_H_

# include <time.h>         // for time_t
# include <string>
# include "events/event.h"

namespace              Events
{
  /**
   *  \class Downtime downtime.h "events/downtime.h"
   *  \brief Represents a downtime inside Nagios.
   *
   *  A Nagios user may have the ability to define downtimes, which are time
   *  periods inside which some host or service shall not generate any
   *  notification. This can occur when an admin sys perform maintenance on a
   *  server for example.
   */
  class                Downtime : public Event
  {
   private:
    void               InternalCopy(const Downtime& downtime);

   public:
    std::string        author;
    std::string        comment;
    short              downtime_type;
    time_t             duration;
    time_t             end_time;
    time_t             entry_time;
    bool               fixed;
    std::string        host_name;
    int                internal_id;
    std::string        instance_name;
    std::string        service_description;
    time_t             start_time;
    int                triggered_by;
    bool               was_cancelled;
    bool               was_started;
                       Downtime();
                       Downtime(const Downtime& downtime);
                       ~Downtime();
    Downtime&          operator=(const Downtime& downtime);
    int                GetType() const;
  };
}

#endif /* !EVENTS_DOWNTIME_H_ */
