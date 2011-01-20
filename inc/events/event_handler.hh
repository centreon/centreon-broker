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
**
** For more information: contact@centreon.com
*/

#ifndef EVENTS_EVENT_HANDLER_HH_
# define EVENTS_EVENT_HANDLER_HH_

# include <string>
# include <sys/types.h>
# include "events/event.hh"

namespace          events {
  /**
   *  @class event_handler event_handler.hh "events/event_handler.hh"
   *  @brief Represents an event handler inside Nagios.
   *
   *  Event handlers, as their name suggests, are executed upon the
   *  detection of some events by Nagios.
   */
  class            event_handler : public event {
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
    int            get_type() const;
  };
}

#endif /* !EVENTS_EVENT_HANDLER_HH_ */
