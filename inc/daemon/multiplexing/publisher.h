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

#ifndef MULTIPLEXING_PUBLISHER_H_
# define MULTIPLEXING_PUBLISHER_H_

# include <list>
# include "concurrency/mutex.h"
# include "interface/destination.h"

// Forward declaration.
namespace                  Events
{ class                    Event; }

namespace                  Multiplexing
{
  // Forward declaration.
  class                    Subscriber;

  /**
   *  \class Publisher publisher.h "multiplexing/publisher.h"
   *  \brief Publish events to registered Subscribers.
   *
   *  The Publisher is a singleton that is responsible of broadcasting events
   *  to every Subscriber which previously registered. The Publisher first
   *  receives events through its Publish() method and then send them to every
   *  Subscriber which Subscribe()'d.
   *
   *  \see Subscriber
   */
  class                    Publisher : public Interface::Destination
  {
   private:
    std::list<Subscriber*> subscribers_;
    Concurrency::Mutex     subscribersm_;
                           Publisher();
                           Publisher(const Publisher& publisher);
                           ~Publisher();
    Publisher&             operator=(const Publisher& publisher);

   public:
    void                   Close();
    void                   Event(Events::Event* event);
    static Publisher&      Instance();
    Subscriber*            Subscribe();
    void                   Unsubscribe(const Subscriber* subscriber);
  };
}

#endif /* !MULTIPLEXING_PUBLISHER_H_ */
