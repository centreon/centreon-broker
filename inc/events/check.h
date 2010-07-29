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

#ifndef EVENTS_CHECK_H_
# define EVENTS_CHECK_H_

# include <string>
# include "events/event.h"

namespace       Events
{
  /**
   *  \class Check check.h "events/check.h"
   *  \brief Check that has been executed.
   *
   *  Once a check has been executed (the check itself, not deduced
   *  information), this kind of event is sent.
   *
   *  \see HostCheck
   *  \see ServiceCheck
   */
  class         Check : public Event
  {
   private:
    void        InternalCopy(const Check& check);

   public:
    std::string command_line;
    int         host_id;
                Check();
                Check(const Check& check);
    virtual     ~Check();
    Check&      operator=(const Check& check);
  };
}

#endif /* !EVENTS_CHECK_H_ */
