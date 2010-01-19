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

#ifndef INTERFACE_NDO_SOURCEDESTINATION_H_
# define INTERFACE_NDO_SOURCEDESTINATION_H_

# include "interface/ndo/destination.h"
# include "interface/ndo/source.h"
# include "interface/sourcedestination.h"

namespace                Interface
{
  namespace              NDO
  {
    /**
     *  \class SourceDestination sourcedestination.h "interface/ndo/sourcedestination.h"
     *  \brief Act as Source and Destination of/for events.
     *
     *  Sum capabilities of Interface::NDO::Source and
     *  Interface::NDO::Destination classes.
     *
     *  \see Interface::NDO::Source
     *  \see Interface::NDO::Destination
     */
    class                SourceDestination : public Interface::SourceDestination,
                                             public Interface::NDO::Source,
                                             public Interface::NDO::Destination
    {
     private:
                         SourceDestination(const SourceDestination& sd);
      SourceDestination& operator=(const SourceDestination& sd);

     public:
                         SourceDestination(IO::Stream* stream);
			 ~SourceDestination();
      void               Close();
    };
  }
}

#endif /* !INTERFACE_NDO_SOURCEDESTINATION_H_ */
