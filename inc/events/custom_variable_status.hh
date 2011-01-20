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

#ifndef EVENTS_CUSTOM_VARIABLE_STATUS_HH_
# define EVENTS_CUSTOM_VARIABLE_STATUS_HH_

# include <string>
# include <time.h>
# include "events/event.hh"

namespace                   events {
  /**
   *  @class custom_variable_status custom_variable_status.hh "events/custom_variable_status.hh"
   *  @brief Custom variable update.
   *
   *  The value of a custom variable has changed.
   */
  class                     custom_variable_status : public event {
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
    virtual int             get_type() const;
  };
}

#endif /* !EVENTS_CUSTOM_VARIABLE_STATUS_HH_ */
