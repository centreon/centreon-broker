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

#ifndef EVENTS_FLAPPING_STATUS_HH_
# define EVENTS_FLAPPING_STATUS_HH_

# include <sys/time.h>
# include "events/event.h"

namespace            Events {
  /**
   *  @class flapping_status flapping_status.hh "events/flapping_status.hh"
   *  @brief Store a flapping status.
   *
   *  Store flapping statuses.
   */
  class              flapping_status : public Event {
   private:
    void             _internal_copy(flapping_status const& fs);

   public:
    time_t           comment_time;
    time_t           event_time;
    int              event_type;
    double           high_threshold;
    int              host_id;
    int              internal_comment_id;
    double           low_threshold;
    double           percent_state_change;
    short            reason_type;
    int              service_id;
    short            type;
                     flapping_status();
                     flapping_status(flapping_status const& fs);
                     ~flapping_status();
    flapping_status& operator=(flapping_status const& fs);
    int              GetType() const;
  };
}

#endif /* !EVENTS_FLAPPING_STATUS_HH_ */
