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

#ifndef INTERFACE_NDO_SOURCE_H_
# define INTERFACE_NDO_SOURCE_H_

# include <memory>             // for auto_ptr
# include "interface/source.h"

// Forward declaration.
namespace                       IO
{ class                         Stream; }

namespace                       Interface
{
  namespace                     NDO
  {
    /**
     *  \class Source source.h "interface/ndo/source.h"
     *  \brief NDO input source.
     *
     *  The Interface::NDO::Source class converts an input stream into events
     *  using the NDO protocol.
     */
    class                       Source : public Interface::Source
    {
     private:
      std::auto_ptr<IO::Stream> stream_;
                                Source(const Source& source);
      Source&                   operator=(const Source& source);

     public:
                                Source(IO::Stream* stream);
      virtual                   ~Source();
      virtual void              Close();
      virtual Events::Event*    Event();
    };
  }
}

#endif /* !INTERFACE_NDO_SOURCE_H_ */
