/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_EVENTS_LOG_ENTRY_HH_
# define CCB_EVENTS_LOG_ENTRY_HH_

# include <QString>
# include <time.h>
# include "events/event.hh"

namespace            com {
  namespace          centreon {
    namespace        broker {
      namespace      events {
        /**
         *  @class log_entry log_entry.hh "events/log_entry.hh"
         *  @brief Nagios-generated log message.
         *
         *  From time to time, the scheduling engine generates a log
         *  message. These messages can be useful, especially when
         *  investigating problems. This class holds all information
         *  related to a log entry.
         */
        class        log_entry : public event {
         private:
          void       _internal_copy(log_entry const& le);

         public:
          time_t     c_time;
          int        host_id;
          QString    host_name;
          QString    instance_name;
          time_t     issue_start_time;
          short      msg_type;
          QString    notification_cmd;
          QString    notification_contact;
          QString    output;
          int        retry;
          QString    service_description;
          int        service_id;
          short      status;
          short      type;
                     log_entry();
                     log_entry(log_entry const& le);
                     ~log_entry();
          log_entry& operator=(const log_entry& le);
          int        get_type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_LOG_ENTRY_HH_ */
