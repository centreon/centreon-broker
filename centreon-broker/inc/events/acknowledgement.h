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

#ifndef EVENTS_ACKNOWLEDGEMENT_H_
# define EVENTS_ACKNOWLEDGEMENT_H_

# include <ctime> // for time_t
# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    /**
     *  \class Acknowledgement acknowledgement.h "events/acknowledgement.h"
     *  \brief Represents an acknowledgement inside Nagios.
     *
     *  When some service or host is critical, Nagios will emit notifications
     *  according to its configuration. To stop the notification process, a
     *  user can acknowledge the problem.
     */
    class                Acknowledgement : public Event
    {
     private:
      void               InternalCopy(const Acknowledgement& ack);

     public:
      std::string        author;
      std::string        comment;
      time_t             entry_time;
      std::string        host;
      bool               is_sticky;
      short              notify_contacts;
      short              persistent_comment;
      std::string        service;
      short              state;
      int                type;
                         Acknowledgement();
                         Acknowledgement(const Acknowledgement& ack);
                         ~Acknowledgement();
      Acknowledgement&   operator=(const Acknowledgement& ack);
      int                GetType() const throw ();
    };
  }
}

#endif /* !EVENTS_ACKNOWLEDGEMENT_H_ */
