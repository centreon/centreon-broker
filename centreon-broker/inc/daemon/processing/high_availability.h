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

#ifndef PROCESSING_HIGH_AVAILABILITY_H_
# define PROCESSING_HIGH_AVAILABILITY_H_

# include <list>
# include <memory>                           // for auto_ptr
# include "concurrency/condition_variable.h"
# include "concurrency/mutex.h"
# include "concurrency/thread_listener.h"
# include "interface/source.h"
# include "multiplexing/subscriber.h"

// Forward declarations.
namespace               Events
{ class                 Event; }
namespace               Interface
{ class                 SourceDestination; }

namespace               Processing
{
  // Forward declaration.
  class                 Delivery;

  /**
   *  \class HighAvailability high_availability.h "processing/high_availability.h"
   *  \brief Provides failover on output.
   *
   *  This class handles an output and can use failover when this output fails.
   */
  class                 HighAvailability : public Concurrency::ThreadListener,
                                           public Interface::Source,
                                           public Multiplexing::Subscriber
  {
   private:
    Delivery*           delivery_;
    std::list<Events::Event*>
                        events_;
    Concurrency::ConditionVariable
                        eventscv_;
    Concurrency::Mutex  eventsm_;
    std::auto_ptr<Interface::SourceDestination>
                        sd_;
                        HighAvailability(const HighAvailability& ha);
    HighAvailability&   operator=(const HighAvailability& ha);

   public:
                        HighAvailability();
                        ~HighAvailability();
    void                Close();
    Events::Event*      Event();
    void                Init(Interface::SourceDestination* sd);
    void                OnEvent(Events::Event* event);
  };
}

#endif /* !PROCESSING_HIGH_AVAILABILITY_H_ */
