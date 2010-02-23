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

#ifndef EVENTS_SERVICE_H_
# define EVENTS_SERVICE_H_

# include <time.h>                  // for time_t
# include <string>
# include "events/host_service.h"
# include "events/service_status.h"

namespace              Events
{
  /**
   *  \class Service service.h "events/service.h"
   *  \brief Service as handled by Nagios.
   *
   *  Holds full data regarding a service.
   *
   *  \see HostService
   *  \see ServiceStatus
   */
  class                Service : public HostService, public ServiceStatus
  {
   private:
    void               InternalCopy(const Service& s);
    void               ZeroInitialize();

   public:
    std::string        failure_prediction_options;
    short              flap_detection_on_critical;
    short              flap_detection_on_ok;
    short              flap_detection_on_unknown;
    short              flap_detection_on_warning;
    bool               is_volatile;
    bool               notified_on_critical;
    bool               notified_on_unknown;
    bool               notified_on_warning;
    std::string        service;
    short              stalk_on_critical;
    short              stalk_on_ok;
    short              stalk_on_unknown;
    short              stalk_on_warning;
                       Service();
                       Service(const ServiceStatus& ss);
                       Service(const Service& s);
                       ~Service();
    Service&           operator=(const Service& s);
    int                GetType() const;
  };
}

#endif /* !EVENTS_SERVICE_H_ */
