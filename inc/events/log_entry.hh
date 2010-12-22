/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef EVENTS_LOG_ENTRY_HH_
# define EVENTS_LOG_ENTRY_HH_

# include <string>
# include <sys/types.h>
# include "events/event.hh"

namespace              events {
  /**
   *  @class log_entry log_entry.hh "events/log_entry.hh"
   *  @brief Nagios-generated log message.
   *
   *  From time to time, Nagios generates a log message. These messages
   *  can be useful, especially when investigating problems. This class
   *  holds all information related to a log entry.
   */
  class                log_entry : public event {
   private:
    void               _internal_copy(log_entry const& le);

   public:
    time_t             c_time;
    int                host_id;
    std::string        host_name;
    std::string        instance_name;
    time_t             issue_start_time;
    short              msg_type;
    std::string        notification_cmd;
    std::string        notification_contact;
    std::string        output;
    int                retry;
    std::string        service_description;
    int                service_id;
    short              status;
    short              type;
                       log_entry();
                       log_entry(log_entry const& le);
                       ~log_entry();
    log_entry&         operator=(const log_entry& le);
    int                get_type() const;
  };
}

#endif /* !EVENTS_LOG_ENTRY_HH_ */
