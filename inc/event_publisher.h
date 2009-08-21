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

#ifndef EVENT_PUBLISHER_H_
# define EVENT_PUBLISHER_H_

# include <boost/thread/mutex.hpp>
# include <list>

namespace                       CentreonBroker
{
  // Forward declarations.
  namespace                     Events
  { class                       Event; }
  class                         EventSubscriber;

  /**
   *  \class EventPublisher event_publisher.h "event_publisher.h"
   *  \brief Publish events to all registered EventSubscribers.
   *
   *  The EventPublisher is a singleton that is responsible of broadcasting
   *  events to every EventSubscriber which registered. The EventPublisher
   *  first receive events via Publish() (usually called from NetworkInput) and
   *  then sends it to every object which Subscribe()'d.
   *
   *  \see EventSubscriber
   *  \see NetworkInput
   */
  class                         EventPublisher
  {
   private:
    std::list<EventSubscriber*> subscribers_;
    boost::mutex                subscribersm_;
                                EventPublisher();
                                EventPublisher(const EventPublisher& ep);
    EventPublisher&             operator=(const EventPublisher& ep);

   public:
                                ~EventPublisher();
    static EventPublisher&      GetInstance();
    void                        Publish(Events::Event* ev);
    void                        Subscribe(EventSubscriber* es);
    void                        Unsubscribe(EventSubscriber* es);
  };
}

#endif /* !EVENT_PUBLISHER_H_ */
