/*
**  Copyright 2010 MERETHIS
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

#ifndef EVENTS_NOTIFICATION_HH_
# define EVENTS_NOTIFICATION_HH_

# include <string>
# include <sys/time.h>
# include "events/event.h"

namespace         Events {
  /**
   *  @class notification notification.hh "events/notification.hh"
   *  @brief Represents a notification inside Nagios.
   *
   *  Notifications are sent by Nagios to notify users of an issue in
   *  their monitored IT infrastructure.
   */
  class           notification : public Event {
   private:
    void          _internal_copy(notification const& n);

   public:
    std::string   ack_author;
    std::string   ack_data;
    std::string   command_name;
    std::string   contact_name;
    bool          contacts_notified;
    time_t        end_time;
    bool          escalated;
    int           host_id;
    std::string   output;
    int           reason_type;
    int           service_id;
    time_t        start_time;
    int           state;
    int           type;
                  notification();
                  notification(notification const& n);
                  ~notification();
    notification& operator=(notification const& n);
    int           GetType() const;
  };
}

#endif /* !EVENTS_NOTIFICATION_HH_ */
