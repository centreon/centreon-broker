/*
** Copyright 2011 Merethis
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

#ifndef CCB_EVENTS_STATUS_DATA_HH_
# define CCB_EVENTS_STATUS_DATA_HH_

# include <time.h>
# include "events/event.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        events {
        /**
         *  @class status_data status_data.hh "events/status_data.hh"
         *  @brief Status data used to generate status graphs.
         *
         *  Status data event used to generate status graphs.
         */
        class          status_data : public event {
         private:
          void         _internal_copy(status_data const& sd);

         public:
          time_t       ctime;
          int          index_id;
          time_t       interval;
          time_t       rrd_len;
          short        status;
                       status_data();
                       status_data(status_data const& sd);
                       ~status_data();
          status_data& operator=(status_data const& sd);
          unsigned int type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_STATUS_DATA_HH_ */
