/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NEB_EVENT_HANDLER_HH_
# define CCB_NEB_EVENT_HANDLER_HH_

# include <QString>
# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          neb {
        /**
         *  @class event_handler event_handler.hh "com/centreon/broker/neb/event_handler.hh"
         *  @brief Represents an event handler inside the scheduling engine.
         *
         *  Event handlers, as their name suggests, are executed upon
         *  the detection of some events by the scheduling engine.
         */
        class            event_handler : public io::data {
         private:
          void           _internal_copy(event_handler const& eh);

         public:
          QString        command_args;
          QString        command_line;
          short          early_timeout;
          time_t         end_time;
          double         execution_time;
          short          handler_type;
          unsigned int   host_id;
          QString        output;
          short          return_code;
          unsigned int   service_id;
          time_t         start_time;
          short          state;
          short          state_type;
          short          timeout;
                         event_handler();
                         event_handler(event_handler const& eh);
                         ~event_handler();
          event_handler& operator=(event_handler const& eh);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_EVENT_HANDLER_HH_ */
