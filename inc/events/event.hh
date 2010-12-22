/*
** Copyright 2009-2010 MERETHIS
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

#ifndef EVENTS_EVENT_HH_
# define EVENTS_EVENT_HH_

# include <string>
# include "concurrency/mutex.hh"

namespace              events {
  /**
   *  @class event event.hh "events/event.hh"
   *  @brief Base class of all events.
   *
   *  The event class represents an event generated in Nagios and then
   *  forwarded to Centreon Broker. event is just an interface that
   *  concrete implementations (like host_status or service_status) have
   *  to follow.
   *
   *  Usually concrete events are dynamically allocated with new,
   *  because when nobody is reading the event anymore, it
   *  self-destructs.
   *
   *  How does the event-dispatching works ? First an input class
   *  generates events and passes them to the multiplexing::publisher
   *  singleton. The publisher holds a list of all
   *  multiplexing::subscriber objects interested in event notification.
   *  It then passes this object to every subscriber which can process
   *  it. When the subscriber is finished with the event, it removes
   *  itself from the event's reader list. When no object is registered
   *  against the event anymore, the event will self-destructs (ie.
   *  delete this;).
   *
   *  @see multiplexing::publisher
   *  @see multiplexing::subscriber
   */
  class                event {
   private:
    concurrency::mutex _mutex;
    int                _readers;

   protected:
                       event(event const& e);
    event&             operator=(event const& e);

   public:
    /**
     *  @brief Enum of event types.
     *
     *  Every event subclass has a corresponding type associated so one
     *  can know its true type through the virtual method get_type().
     *
     *  Beware when modifying this enum. Changes should be replicated on
     *  multiple files. Here's the list :
     *
     *    - src/correlation/correlator.cc        : dispatch table
     *    - src/interface/db/                    : handle event
     *    - src/interface/ndo/                   : handle event
     *    - src/interface/xml/                   : handle event
     *    - src/mapping.cc                       : members and type mappings
     *    - src/module/{callbacks.cc|initial.cc} : generate event
     *
     *  @see get_type
     */
    enum               type {
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
      HOSTSTATUS,
      ISSUE,
      ISSUEPARENT,
      LOG,
      NOTIFICATION,
      PROGRAM,
      PROGRAMSTATUS,
      SERVICE,
      SERVICECHECK,
      SERVICEDEPENDENCY,
      SERVICEGROUP,
      SERVICEGROUPMEMBER,
      SERVICESTATUS,
      STATE,
      EVENT_TYPES_NB
    };
                       event();
    virtual            ~event();
    void               add_reader();
    virtual int        get_type() const = 0;
    void               remove_reader();
  };
}

#endif /* !EVENTS_EVENT_HH_ */
