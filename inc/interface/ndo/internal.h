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

#ifndef INTERFACE_NDO_INTERNAL_H_
# define INTERFACE_NDO_INTERNAL_H_

# include <map>
# include <sstream>
# include "events/events.h"
# include "mapping.h"

namespace                  Interface
{
  namespace                NDO
  {
    template               <typename T>
    struct                 GetterSetter
    {
      const DataMember<T>* member;
      void                 (* getter)(const T&,
                                      const DataMember<T>&,
                                      std::stringstream& buffer);
      void                 (* setter)(T&, const DataMember<T>&, const char*);
    };

    // NDO mappings.
    template               <typename T>
    struct                 NDOMappedType
    {
      static std::map<int, GetterSetter<T> > map;
    };

    // Mapping initialization routine.
    void Initialize();
  }
}

#endif /* !INTERFACE_NDO_INTERNAL_H_ */
