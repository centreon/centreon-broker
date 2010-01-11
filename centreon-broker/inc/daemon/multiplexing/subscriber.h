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

# include <list>
# include <time.h>                           // for time_t
# include "concurrency/condition_variable.h"
# include "concurrency/mutex.h"
# include "interface/destination.h"
# include "interface/source.h"

namespace                      Multiplexing
{
  /**
   *  \class Subscriber subscriber.h "multiplexing/subscriber.h"
   *  \brief Receive events from the Publisher and make them available through
   *         the Interface::Source interface.
   *
   *  This class is used as a cornerstone in event multiplexing. Each output
   *  willing to receive events will request a Subscriber object from the
   *  Publisher which will broadcast events to every Subscriber object when it
   *  receives one.
   *
   *  \see Publisher
   */
  class                        Subscriber : public Interface::Destination,
                                            public Interface::Source
  {
    friend class               Multiplexing::Publisher;

   private:
    mutable Concurrency::ConditionVariable
                               cv_;
    mutable Concurrency::Mutex mutex_;
    std::list<Events::Event*>  events_;
                               Subscriber();
                               Subscriber(const Subscriber& subscriber);
    Subscriber&                operator=(const Subscriber& subscriber);
    void                       Clean();
    void                       InternalCopy(const Subscriber& subscr);

   public:
                               ~Subscriber();
    void                       Close();
    Events::Event*             Event();
    Events::Event*             Event(time_t deadline);
    void                       Event(Events::Event* event);
  };
}

#endif /* !MULTIPLEXING_SUBSCRIBER_H_ */
