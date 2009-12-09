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

#ifndef PROCESSING_DELIVERY_H_
# define PROCESSING_DELIVERY_H_

# include "concurrency/thread.h"

// Forward declarations.
namespace                   Concurrency
{ class                     ThreadListener; }
namespace                   Interface
{
  class                     Destination;
  class                     Source;
}

namespace                   Processing
{
  /**
   *  \class Delivery delivery.h "processing/delivery.h"
   *  \brief Transfer events from a HighAvailability object to a destination.
   *
   *  Transfer events from a HighAvailability to a destination.
   */
  class                     Delivery : public Concurrency::Thread
  {
   private:
    Interface::Destination* dest_;
    Interface::Source*      source_;
                            Delivery(const Delivery& delivery);
    Delivery&               operator=(const Delivery& delivery);

   public:
                            Delivery();
                            ~Delivery();
    void                    operator()();
    void                    Init(Interface::Source* source,
                                 Interface::Destination* destination,
                                 Concurrency::ThreadListener* tl = NULL);
  };
}

#endif /* !PROCESSING_DELIVERY_H_ */
