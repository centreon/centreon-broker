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

#ifndef INTERFACE_DESTINATION_H_
# define INTERFACE_DESTINATION_H_

// Forward declaration.
namespace          Events
{ class            Event; }

namespace          Interface
{
  /**
   *  \class Destination destination.h "interface/destination.h"
   *  \brief Base interface for event-storing objects.
   *
   *  Interface::Destination is the base interface used to store objects in an
   *  output destination. The underlying destination can either be a XML
   *  stream, a database, ...
   *
   *  \see DB::DB
   *  \see File::File
   *  \see NDO::NDO
   */
  class          Destination
  {
   protected:
                 Destination();
                 Destination(const Destination& dest);
    Destination& operator=(const Destination& dest);

   public:
    virtual      ~Destination();
    virtual void Close() = 0;
    virtual void Event(Events::Event* event) = 0;
  };
}

#endif /* !INTERFACE_DESTINATION_H_ */
