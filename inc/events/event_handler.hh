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

#ifndef EVENTS_EVENT_HANDLER_HH_
# define EVENTS_EVENT_HANDLER_HH_

# include <string>
# include <time.h>
# include "events/event.h"

namespace          Events {
  /**
   *  @class event_handler event_handler.hh "events/event_handler.hh"
   *  @brief Represents an event handler inside Nagios.
   *
   *  Event handlers, as their name suggests, are executed upon the
   *  detection of some events by Nagios.
   */
  class            event_handler : public Event {
   private:
    void           _internal_copy(event_handler const& eh);

   public:
    std::string    command_args;
    std::string    command_line;
    short          early_timeout;
    time_t         end_time;
    double         execution_time;
    int            host_id;
    std::string    output;
    short          return_code;
    int            service_id;
    time_t         start_time;
    short          state;
    short          state_type;
    short          timeout;
    short          type;
                   event_handler();
                   event_handler(event_handler const& eh);
                   ~event_handler();
    event_handler& operator=(event_handler const& eh);
    int            GetType() const;
  };
}

#endif /* !EVENTS_EVENT_HANDLER_HH_ */
