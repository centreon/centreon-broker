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

# include <memory>
# include "interface/destination.h"

namespace                  Multiplexing
{
  /**
   *  \class Publisher publisher.h "multiplexing/publisher.h"
   *  \brief Publish events to registered Subscribers.
   *
   *  A Publisher object broadcast an event sent to it to every Subscriber.
   *
   *  \see Subscriber
   */
  class                    Publisher : public Interface::Destination
  {
   public:
                           Publisher();
                           Publisher(const Publisher& publisher);
                           ~Publisher();
    Publisher&             operator=(const Publisher& publisher);
    void                   Close();
    static void            Correlate();
    void                   Event(Events::Event* event);
  };
}

#endif /* !MULTIPLEXING_PUBLISHER_H_ */
