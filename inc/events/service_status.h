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

#ifndef EVENTS_SERVICE_STATUS_H_
# define EVENTS_SERVICE_STATUS_H_

# include <time.h>                       // for time_t
# include <string>
# include "events/host_service_status.h"

namespace               Events
{
  class                 EventSubscriber;

  /**
   *  \class ServiceStatus service_status.h "events/service_status.h"
   *  \brief When the status of a service change, such an event is generated.
   *
   *  This class represents a change in a service status.
   */
  class                ServiceStatus : public HostServiceStatus
  {
   private:
    void               InternalCopy(const ServiceStatus& ss);

   public:
    time_t             last_time_critical;
    time_t             last_time_ok;
    time_t             last_time_unknown;
    time_t             last_time_warning;
    std::string        service;
    int                service_id;
                       ServiceStatus();
                       ServiceStatus(const ServiceStatus& ss);
    virtual            ~ServiceStatus();
    ServiceStatus&     operator=(const ServiceStatus& ss);
    int                GetType() const;
  };
}

#endif /* !EVENTS_SERVICE_STATUS_H_ */
