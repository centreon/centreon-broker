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

#ifndef INTERFACE_SOURCEDESTINATION_H_
# define INTERFACE_SOURCEDESTINATION_H_

# include "interface/destination.h"
# include "interface/source.h"

namespace              Interface
{
  /**
   *  \class SourceDestination sourcedestination.h "interface/sourcedestination.h"
   *  \brief Base interface for objects that can generate and store events.
   *
   *  SourceDestination is at the same time a Source and a Destination. It
   *  regroups the functionnality of both classes within one single interface.
   *
   *  \see Source
   *  \see Destination
   */
  class                SourceDestination : virtual public Interface::Source,
                                           virtual public Interface::Destination
  {
   protected:
                       SourceDestination();
                       SourceDestination(const SourceDestination& sd);
    SourceDestination& operator=(const SourceDestination& sd);

   public:
    virtual            ~SourceDestination();
    virtual void       Close() = 0;
  };
}

#endif /* !INTERFACE_SOURCEDESTINATION_H_ */
