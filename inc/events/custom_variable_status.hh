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

#ifndef EVENTS_CUSTOM_VARIABLE_STATUS_HH_
# define EVENTS_CUSTOM_VARIABLE_STATUS_HH_

# include <string>
# include <time.h>
# include "events/event.h"

namespace                   Events {
  /**
   *  @class custom_variable_status custom_variable_status.hh "events/custom_variable_status.hh"
   *  @brief Custom variable update.
   *
   *  The value of a custom variable has changed.
   */
  class                     custom_variable_status : public Event {
   private:
    void                    _internal_copy(custom_variable_status const& cvs);

   public:
    int                     host_id;
    bool                    modified;
    std::string             name;
    int                     service_id;
    time_t                  update_time;
    std::string             value;
                            custom_variable_status();
                            custom_variable_status(custom_variable_status const& cvs);
    virtual                 ~custom_variable_status();
    custom_variable_status& operator=(custom_variable_status const& cvs);
    virtual int             GetType() const;
  };
}

#endif /* !EVENTS_CUSTOM_VARIABLE_STATUS_HH_ */
