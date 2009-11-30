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

#ifndef INTERFACE_SOURCE_SOURCE_H_
# define INTERFACE_SOURCE_SOURCE_H_

// Forward declaration.
namespace                    Events
{ class                      Event; }

namespace                    Interface
{
  namespace                  Source
  {
    /**
     *  \class Source source.h "interface/source/source.h"
     *  \brief Base interface for event-generating objects.
     *
     *  Interface::Source::Source is the base interface used to get objects
     *  from an input source. The underlying source can be either a XML stream,
     *  a database, ...
     *
     *  \see DB
     *  \see File
     *  \see NDO
     *  \see XML
     */
    class                    Source
    {
     protected:
                             Source();
                             Source(const Source& source);
      Source&                operator=(const Source& source);

     public:
      virtual                ~Source();
      virtual void           Close() = 0;
      virtual Events::Event* Event() = 0;
    };
  }
}

#endif /* !INTERFACE_SOURCE_SOURCE_H_ */
