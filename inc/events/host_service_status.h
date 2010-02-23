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

#ifndef EVENTS_HOST_SERVICE_STATUS_H_
# define EVENTS_HOST_SERVICE_STATUS_H_

# include <time.h>          // for time_t
# include <string>
# include "events/status.h"

namespace              Events
{
  /**
   *  \class HostServiceStatus host_service_status.h "events/host_service_status.h"
   *  \brief HostStatus and ServiceStatus shared data.
   *
   *  This class represents what is shared between a host status event and a
   *  service status event.
   *
   *  \see HostStatus
   *  \see ServiceStatus
   */
  class                HostServiceStatus : public Status
  {
   private:
    void               InternalCopy(const HostServiceStatus &hss);

   public:
    short              acknowledgement_type;
    bool               active_checks_enabled;
    std::string        check_command;
    double             check_interval;
    std::string        check_period;
    short              check_type;
    short              current_check_attempt;
    short              current_notification_number;
    short              current_state;
    std::string        event_handler;
    double             execution_time;
    bool               has_been_checked;
    int                id;
    bool               is_flapping;
    time_t             last_check;
    short              last_hard_state;
    time_t             last_hard_state_change;
    time_t             last_notification;
    time_t             last_state_change;
    time_t             last_update;
    double             latency;
    std::string        long_output;
    short              max_check_attempts;
    int                modified_attributes;
    time_t             next_check;
    time_t             next_notification;
    bool               no_more_notifications;
    bool               obsess_over;
    std::string        output;
    bool               passive_checks_enabled;
    double             percent_state_change;
    std::string        perf_data;
    bool               problem_has_been_acknowledged;
    double             retry_interval;
    short              scheduled_downtime_depth;
    bool               should_be_scheduled;
    short              state_type;
                       HostServiceStatus();
                       HostServiceStatus(const HostServiceStatus& hss);
    virtual            ~HostServiceStatus();
    HostServiceStatus& operator=(const HostServiceStatus& hss);
  };
}

#endif /* !EVENTS_HOST_SERVICE_STATUS_H_ */
