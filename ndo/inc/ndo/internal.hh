/*
** Copyright 2009-2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NDO_INTERNAL_HH_
# define CCB_NDO_INTERNAL_HH_

# include <map>
# include <sstream>
# include "mapping.hh"

namespace                       com {
  namespace                     centreon {
    namespace                   broker {
      namespace                 ndo {
        template                <typename T>
        struct                  getter_setter {
          data_member<T> const* member;
          void                  (* getter)(T const&,
                                  data_member<T> const&,
                                  std::stringstream& buffer);
          void                  (* setter)(T&,
                                  data_member<T> const&,
                                  char const*);
        };

        // NDO mappings.
        template                <typename T>
        struct                  ndo_mapped_type {
          static std::map<int, getter_setter<T> > map;
        };

        // Mapping initialization routine.
        void initialize();
      }
    }
  }
}

#endif /* !CCB_NDO_INTERNAL_HH_ */
