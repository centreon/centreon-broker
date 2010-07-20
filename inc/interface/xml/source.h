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

#ifndef EVENTS_XML_SOURCE_H_
# define EVENTS_XML_SOURCE_H_

# include <memory>
# include "interface/source.h"
# include "io/stream.h"

namespace                       Interface
{
  namespace                     XML
  {
    /**
     *  \class Source source.h "interface/xml/source.h"
     *  \brief XML input source.
     *
     *  The Interface::XML::Source class converts an input stream into events
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

#endif /* !EVENTS_XML_SOURCE_H_ */
