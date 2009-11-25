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

#ifndef MULTIPLEXING_SUBSCRIBER_H_
# define MULTIPLEXING_SUBSCRIBER_H_

// Forward declaration.
namespace        Events
{ class          Event; }

namespace        Multiplexing
{
  /**
   *  \class Subscriber subscriber.h "multiplexing/subscriber.h"
   *  \brief A Subscriber can receive events published by the Publisher.
   *
   *  A Subscriber can subscribe against the Publisher to receive events when
   *  they occur. The process is simple : at startup the Subscriber subscribes
   *  against the Publisher and will then be called through its OnEvent()
   *  method whenever an event occurs.
   *
   *  \see Publisher
   */
  class          Subscriber
  {
   protected:
                 Subscriber();
                 Subscriber(const Subscriber& subscriber);
    Subscriber&  operator=(const Subscriber& subscriber);

   public:
    virtual      ~Subscriber();
    virtual void OnEvent(Events::Event* e) = 0;
  };
}

#endif /* !MULTIPLEXING_SUBSCRIBER_H_ */
