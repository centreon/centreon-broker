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

# include <string>
# include "interface/source.h"
# include "io/text.h"

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
      std::string               instance_;
      IO::Text                  stream_;
                                Source(const Source& source);
      Source&                   operator=(const Source& source);
      Events::Event*            Header();

     public:
                                Source(IO::Stream* stream);
      virtual                   ~Source();
      virtual void              Close();
      virtual Events::Event*    Event();
      static void               Initialize();
    };
  }
}

#endif /* !INTERFACE_NDO_SOURCE_H_ */
