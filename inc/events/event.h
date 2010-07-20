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

# include <string>
# include "concurrency/mutex.h"

namespace              Events
{
  /**
   *  \class Event event.h "events/event.h"
   *  \brief Base class of all events.
   *
   *  The Event class represents an event generated in Nagios and then
   *  forwarded to CentreonBroker. Event is just an interface that concrete
   *  implementations (like HostStatus or ServiceStatus) have to follow.
   *
   *  Usually concrete Events are dynamically allocated with new, because when
   *  nobody is reading the Event anymore, it self-destructs.
   *
   *  How does the event-dispatching works ? First an input class generates
   *  events and passes them to the Multiplexing::Publisher singleton. The
   *  Publisher holds a list of all Multiplexing::Subscriber objects interested
   *  in event notification. It then passes this object to every Subscriber
   *  which can process it. When the Subscriber is finished with the event, it
   *  removes itself from the event's reader list. When no object is registered
   *  against the event anymore, the event will self-destructs (ie.
   *  delete this;).
   *
   *  \see Multiplexing::Publisher
   *  \see Multiplexing::Subscriber
   */
  class                Event
  {
   private:
    Concurrency::Mutex mutex_;
    int                readers_;

   protected:
                       Event(const Event& event);
    Event&             operator=(const Event& event);

   public:
    /**
     *  \brief Enum of event types.
     *
     *  Every Event subclass has a corresponding Type associated so one can
     *  know its true type through the virtual method GetType().
     *
     *  Beware when modifying this enum. Changes should be replicated on
     *  multiple files. Here's the list :
     *
     *    - src/correlation/correlator.cpp         : dispatch table
     *    - src/interface/db/                      : handle event
     *    - src/interface/ndo/                     : handle event
     *    - src/interface/xml/                     : handle event
     *    - src/mapping.cpp                        : members and type mappings
     *    - src/module/{callbacks.cpp|initial.cpp} : generate event
     *
     *  \see GetType
     */
    enum               Type
    {
      UNKNOWN = 0,
      ACKNOWLEDGEMENT,
      COMMENT,
      DOWNTIME,
      HOST,
      HOSTCHECK,
      HOSTDEPENDENCY,
      HOSTGROUP,
      HOSTGROUPMEMBER,
      HOSTPARENT,
      HOSTSTATUS,
      ISSUE,
      ISSUEUPDATE,
      LOG,
      PROGRAM,
      PROGRAMSTATUS,
      SERVICE,
      SERVICECHECK,
      SERVICEDEPENDENCY,
      SERVICEGROUP,
      SERVICEGROUPMEMBER,
      SERVICESTATUS,
      EVENT_TYPES_NB
    };
                       Event();
    virtual            ~Event();
    void               AddReader();
    virtual int        GetType() const = 0;
    void               RemoveReader();
  };
}

#endif /* !EVENTS_EVENT_H_ */
