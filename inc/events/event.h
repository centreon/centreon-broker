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

#ifndef EVENTS_EVENT_H_
# define EVENTS_EVENT_H_

# include <boost/thread/mutex.hpp>
# include <string>

namespace                CentreonBroker
{
  class                  EventSubscriber; // Forward declaration

  namespace              Events
  {
    /**
     *  \class Event event.h "events/event.h"
     *  \brief Base class of all events.
     *
     *  The Event class represents an event generated in Nagios and then
     *  forwarded to CentreonBroker. Event is just an interface that concrete
     *  implementations (like HostStatusEvent or ServiceStatusEvent) have to
     *  follow.
     *
     *  Usually concrete Events are dynamically allocated with new, because
     *  when nobody is reading the Event anymore, it self-destructs.
     *
     *  How does the event-dispatching works ? First an input class like
     *  NetworkInput generates events and passes them to the EventPublisher
     *  singleton. The EventPublisher holds a list of all EventSubscriber
     *  objects interested in event notification. It then passes this object to
     *  every EventSubscriber which can process it. When the EventSubscriber is
     *  finished with the event, it removes itself from the event's reader
     *  list. When no object is registered against the event anymore, the
     *  event will self-destructs (ie. delete this;).
     *
     *  \see DBOutput
     *  \see EventPublisher
     *  \see EventSubscriber
     *  \see NetworkInput
     */
    class                Event
    {
     private:
      boost::mutex       mutex_;
      int                readers_;

     public:
      /**
       *  \brief Enum of event types.
       *
       *  Every Event subclass has a corresponding Type associated so one can
       *  know its true type through the virtual method GetType().
       *
       *  \see GetType
       */
      enum               Type
      {
	ACKNOWLEDGEMENT = 1,
	COMMENT,
	CONNECTION,
	CONNECTIONSTATUS,
	DOWNTIME,
	HOST,
	HOSTGROUP,
	HOSTSTATUS,
	LOG,
	PROGRAMSTATUS,
	SERVICE,
	SERVICESTATUS
      };
      std::string        instance;
                         Event();
                         Event(const Event& event);
      virtual            ~Event();
      Event&             operator=(const Event& event);
      void               AddReader(EventSubscriber* es);
      virtual int        GetType() const throw () = 0;
      void               RemoveReader(EventSubscriber* es);
    };
  }
}

#endif /* !EVENTS_EVENT_H_ */
