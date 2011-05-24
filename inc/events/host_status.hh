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

#ifndef CCB_EVENTS_HOST_STATUS_HH_
# define CCB_EVENTS_HOST_STATUS_HH_

# include <time.h>
# include "events/host_service_status.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        events {
        /**
         *  @class host_status host_status.hh "events/host_status.hh"
         *  @brief host_status represents a status change of an host.
         *
         *  host_status are generated when the status of an host change.
         *  Appropriate fields should be updated.
         */
        class          host_status : public host_service_status {
         private:
          void         _internal_copy(host_status const& hs);

         public:
          time_t       last_time_down;
          time_t       last_time_unreachable;
          time_t       last_time_up;
                       host_status();
                       host_status(host_status const& hs);
          virtual      ~host_status();
          host_status& operator=(host_status const& hs);
          int          get_type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_HOST_STATUS_HH_ */
