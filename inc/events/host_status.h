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

#ifndef EVENTS_HOST_STATUS_H_
# define EVENTS_HOST_STATUS_H_

# include <time.h>                       // for time_t
# include "events/host_service_status.h"

namespace              Events
{
  class                EventSubscriber;

  /**
   *  \class HostStatus host_status.h "events/host_status.h"
   *  \brief HostStatus represents a status change of an host.
   *
   *  HostStatus are generated when the status of an host change. Appropriate
   *  fields should be updated.
   */
  class                HostStatus : public HostServiceStatus
  {
   private:
    void               InternalCopy(const HostStatus& hs);

   public:
    time_t             last_time_down;
    time_t             last_time_unreachable;
    time_t             last_time_up;
                       HostStatus();
                       HostStatus(const HostStatus& hs);
    virtual            ~HostStatus();
    HostStatus&        operator=(const HostStatus& hs);
    int                GetType() const;
  };
}

#endif /* !EVENTS_HOST_STATUS_H_ */
