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

# include <memory>               // for auto_ptr
# include "concurrency/thread.h"

// Forward declarations.
namespace               Configuration
{ class                 Interface; }
namespace               Concurrency
{ class                 ThreadListener; }
namespace               Interface
{ class                 Destination;
  class                 Source; }

namespace               Processing
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
  class                 Feeder : public Concurrency::Thread
  {
   private:
    Interface::Destination*
                        dest_;
    std::auto_ptr<Configuration::Interface>
                        dest_conf_;
    Interface::Source*  source_;
    std::auto_ptr<Configuration::Interface>
                        source_conf_;
                        Feeder(const Feeder& feeder);
    Feeder&             operator=(const Feeder& feeder);

   public:
                        Feeder();
                        ~Feeder();
    void                operator()();
    void                Run(const Configuration::Interface& source,
                            const Configuration::Interface& dest,
                            Concurrency::ThreadListener* listener = NULL);
    void                Run(const Configuration::Interface& source,
                            Interface::Destination& dest,
                            Concurrency::ThreadListener* listener = NULL);
    void                Run(Interface::Source& source,
                            const Configuration::Interface& dest,
                            Concurrency::ThreadListener* listener = NULL);
    void                Run(Interface::Source& source,
                            Interface::Destination& dest,
                            Concurrency::ThreadListener* listener = NULL);
    //void                UpdateSource(const Configuration::Interface& source);
    //void                UpdateDestination(const Configuration::Interface& d);
  };
}

#endif /* !PROCESSING_FEEDER_H_ */
