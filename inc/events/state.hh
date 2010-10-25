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

#ifndef EVENTS_STATE_HH_
# define EVENTS_STATE_HH_

# include <sys/types.h>
# include "events/event.h"

namespace  Events {
  /**
   *  @class state state.h "events/state.h"
   *  @brief State of a checkpoint.
   *
   *  This class represent the state of a specific checkpoint for a given time.
   */
  class    state : public Event {
   private:
    void   _internal_copy(state const& s);

   public:
    int    current_state;
    time_t end_time;
    int    host_id;
    int    service_id;
    time_t start_time;
           state();
           state(state const& s);
           ~state();
    state& operator=(state const& s);
    int    GetType() const;
  };
}

#endif /* !EVENTS_STATE_HH_ */
