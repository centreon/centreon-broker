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

#ifndef INTERFACE_XML_INTERNAL_H_
# define INTERFACE_XML_INTERNAL_H_

# include <map>
# include <string>
# include "mapping.h"
# include "interface/xml/tinyxml.h"

namespace                  Interface
{
  namespace                XML
  {
    template               <typename T>
    struct                 GetterSetter
    {
      const DataMember<T>* member;
      void                 (* getter)(const T&,
                                      const std::string& name,
                                      const DataMember<T>&,
                                      TiXmlElement& elem);
    };

    // XML mappings.
    template               <typename T>
    struct                 XMLMappedType
    {
      static std::map<std::string, GetterSetter<T> > map;
    };

    // Mapping initialization routine.
    void Initialize();
  }
}

#endif /* !INTERFACE_XML_INTERNAL_H_ */
