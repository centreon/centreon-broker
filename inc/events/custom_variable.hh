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

#ifndef EVENTS_CUSTOM_VARIABLE_HH_
# define EVENTS_CUSTOM_VARIABLE_HH_

# include "events/custom_variable_status.hh"

namespace            Events {
  /**
   *  @class custom_variable custom_variable.hh "events/custom_variable.hh"
   *  @brief Custom variable definition.
   *
   *  Nagios allows users to declare variables attached to a specific
   *  host or service.
   */
  class              custom_variable : public custom_variable_status {
   private:
    void             _internal_copy(custom_variable const& cv);

   public:
    short            type;
                     custom_variable();
                     custom_variable(custom_variable const& cv);
                     ~custom_variable();
    custom_variable& operator=(custom_variable const& cv);
    int              GetType() const;
  };
}

#endif /* !EVENTS_CUSTOM_VARIABLE_HH_ */
