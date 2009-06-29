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

#ifndef EVENT_H_
# define EVENT_H_

# include <boost/thread/mutex.hpp>
# include <string>

namespace              CentreonBroker
{
  class                EventSubscriber;

  /**
   *  The Event class represents an event generated in Nagios and then
   *  forwarded to CentreonBroker. Event is just an interface that concrete
   *  implementations (like HostStatusEvent or ServiceStatusEvent) have to
   *  follow.
   *
   *  Currently all concrete Events have to be dynamically allocated with new,
   *  because when nobody is reading the Event anymore, it self-destructs.
   */
  class                Event
  {
   private:
    boost::mutex       mutex_;
    std::string        nagios_instance_;
    int                readers_;

   public:
    enum               Type
    {
      ACKNOWLEDGEMENT = 1,
      COMMENT,
      CONNECTION,
      CONNECTIONSTATUS,
      DOWNTIME,
      HOST,
      HOSTSTATUS,
      PROGRAMSTATUS,
      SERVICE,
      SERVICESTATUS
    };
                       Event();
                       Event(const Event& event);
    virtual            ~Event();
    Event&             operator=(const Event& event);
    void               AddReader(EventSubscriber* es);
    const std::string& GetNagiosInstance() const throw ();
    virtual int        GetType() const throw () = 0;
    void               RemoveReader(EventSubscriber* es);
    void               SetNagiosInstance(const std::string& inst);
  };
}

#endif /* !EVENT_H_ */
