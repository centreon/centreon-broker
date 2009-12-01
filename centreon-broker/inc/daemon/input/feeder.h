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

#ifndef INPUT_FEEDER_H_
# define INPUT_FEEDER_H_

# include <memory>               // for auto_ptr
# include "concurrency/thread.h"

// Forward declaration.
namespace               Interface
{ class                 Source; }

namespace               Input
{
  /**
   *  \class Feeder feeder.h "input/feeder.h"
   *  \brief Get Events from a source and bring them to the Publisher.
   *
   *  The Feeder class implements the mediator pattern and avoid event sources
   *  to be tightly coupled with the event publisher.
   *
   *  \see Events::Event
   *  \see Multiplexing::Publisher
   */
  class                 Feeder
  {
   private:
    std::auto_ptr<Interface::Source>
                        source_;
    Concurrency::Thread thread_;
                        Feeder(const Feeder& feeder);
    Feeder&             operator=(const Feeder& feeder);

   public:
                        Feeder();
                        ~Feeder();
    void                operator()();
    void                Init(Interface::Source* source);
  };
}

#endif /* !INPUT_FEEDER_H_ */
