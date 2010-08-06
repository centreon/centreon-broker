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

#ifndef PROCESSING_FEEDER_H_
# define PROCESSING_FEEDER_H_

# include <memory>
# include "concurrency/mutex.h"
# include "concurrency/thread.h"
# include "interface/sourcedestination.h"

namespace              Processing
{
  /**
   *  \class Feeder feeder.h "processing/feeder.h"
   *  \brief Get Events from a source and bring it to a destination.
   *
   *  The Feeder class implements the mediator pattern and avoid event sources
   *  to be tightly coupled with the event publisher.
   *
   *  \see Events::Event
   *  \see Multiplexing::Publisher
   */
  class                Feeder : public Concurrency::Thread
  {
   private:
                       Feeder(const Feeder& feeder);
    Feeder&            operator=(const Feeder&feeder);

   public:
                       Feeder();
    virtual            ~Feeder();
    void               Feed(Interface::Source* source,
                            Interface::Destination* dest);
  };

  /**
   *  \class FeederOnce feeder.h "processing/feeder.h"
   *  \brief Get Events from a source and bring it to a destination with no
   *         failover.
   *
   *  Act on two already opened interface and send events from the source to
   *  the destination. If an error occur both interfaces are destroyed.
   */
  class                FeederOnce : public Feeder,
                                    public Interface::SourceDestination
  {
   private:
    std::auto_ptr<Interface::Destination>
                       dest_;
    Concurrency::Mutex destm_;
    std::auto_ptr<Interface::Source>
                       source_;
    Concurrency::Mutex sourcem_;
                       FeederOnce(const FeederOnce& fo);
    FeederOnce&        operator=(const FeederOnce& fo);

   public:
                       FeederOnce();
    virtual            ~FeederOnce();
    virtual void       operator()();
    void               Close();
    Events::Event*     Event();
    void               Event(Events::Event* event);
    void               Exit();
    void               Run(Interface::Source* source,
                           Interface::Destination* dest,
                           Concurrency::ThreadListener* tl = NULL);
  };
}

#endif /* !PROCESSING_FEEDER_H_ */
