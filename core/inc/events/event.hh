/*
** Copyright 2009-2011 Merethis
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
*/

#ifndef CCB_EVENTS_EVENT_HH_
# define CCB_EVENTS_EVENT_HH_

# include "io/data.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        events {
        /**
         *  @class event event.hh "events/event.hh"
         *  @brief Base class of all events.
         *
         *  The event class represents an event generated in the
         *  scheduling engine and then forwarded to Centreon Broker.
         *  event is just an interface that concrete implementations
         *  (like host_status or service_status) have to follow.
         *
         *  Usually concrete events are dynamically allocated with new,
         *  because when nobody is reading the event anymore, it
         *  self-destructs.
         *
         *  How does the event-dispatching works ? First an input class
         *  generates events and passes them to the
         *  multiplexing::publisher singleton. The publisher holds a
         *  list of all multiplexing::subscriber objects interested in
         *  event notification. It then passes this object to every
         *  subscriber which can process it. When the subscriber is
         *  finished with the event, it removes itself from the event's
         *  reader list. When no object is registered against the event
         *  anymore, the event will self-destructs (ie. delete this;).
         *
         *  @see multiplexing::publisher
         *  @see multiplexing::subscriber
         */
        class          event : public io::data {
         protected:
                       event(event const& e);
          event&       operator=(event const& e);

         public:
          /**
           *  @brief Enum of event types.
           *
           *  Every event subclass has a corresponding type associated
           *  so one can know its true type through the virtual method
           *  get_type().
           *
           *  Beware when modifying this enum. Changes should be
           *  replicated on multiple files. Here's the list :
           *
           *    - inc/events/events.hh                 : include all event objects
           *    - src/correlation/correlator.cc        : dispatch table
           *    - src/mapping.cc                       : members and type mappings
           *    - src/module/{callbacks.cc|initial.cc} : generate event
           *
           *  @see type()
           */
          enum {
            UNKNOWN = 0,
            ACKNOWLEDGEMENT,
            COMMENT,
            CUSTOMVARIABLE,
            CUSTOMVARIABLESTATUS,
            DOWNTIME,
            EVENTHANDLER,
            FLAPPINGSTATUS,
            HOST,
            HOSTCHECK,
            HOSTDEPENDENCY,
            HOSTGROUP,
            HOSTGROUPMEMBER,
            HOSTPARENT,
            HOSTSTATE,
            HOSTSTATUS,
            INSTANCE,
            INSTANCESTATUS,
            ISSUE,
            ISSUEPARENT,
            LOG,
            MODULE,
            NOTIFICATION,
            PERFDATA,
            SERVICE,
            SERVICECHECK,
            SERVICEDEPENDENCY,
            SERVICEGROUP,
            SERVICEGROUPMEMBER,
            SERVICESTATE,
            SERVICESTATUS,
            EVENT_TYPES_NB
          };
                       event();
          virtual      ~event();
          void*        memory();
          void const*  memory() const;
          unsigned int size() const;
        };
      }
    }
  }
}

#endif /* !EVENTS_EVENT_HH_ */
