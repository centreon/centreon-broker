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

#ifndef EVENT_SUBSCRIBER_H_
# define EVENT_SUBSCRIBER_H_

# include <string>
# include <sys/types.h>

namespace            CentreonBroker
{
  namespace          Events
  {
    class            Event;
  }

  /**
   *  An EventSubscriber can subscribe against the EventPublisher to receive
   *  events when they occur. It can then access data from the Event using the
   *  Visitor pattern : the subscriber call the AcceptVisitor() method of the
   *  Event which will in return call the overloaded Visit() method.
   */
  class              EventSubscriber
  {
   public:
                     EventSubscriber();
                     EventSubscriber(const EventSubscriber& es);
    virtual          ~EventSubscriber();
    EventSubscriber& operator=(const EventSubscriber& es);
    virtual void     OnEvent(Events::Event* e) = 0;
  };
}

#endif /* !EVENT_SUBSCRIBER_H_ */
