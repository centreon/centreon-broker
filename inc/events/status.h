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

#ifndef EVENTS_STATUS_H_
# define EVENTS_STATUS_H_

# include "events/event.h"

namespace       Events
{
  /**
   *  \class Status status.h "events/status.h"
   *  \brief Root class of status events.
   *
   *  This is the root class of status events : host, program and service
   *  status events.
   *
   *  \see HostStatus
   *  \see ProgramStatus
   *  \see ServiceStatus
   */
  class         Status : public Event
  {
   private:
    void        InternalCopy(const Status& s);

   public:
    bool        event_handler_enabled;
    bool        failure_prediction_enabled;
    bool        flap_detection_enabled;
    bool        notifications_enabled;
    bool        process_performance_data;
                Status();
                Status(const Status& s);
    virtual     ~Status();
    Status&     operator=(const Status& s);
  };
}

#endif /* !EVENTS_STATUS_H_ */
