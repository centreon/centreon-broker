/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef INTERFACE_XML_INTERNAL_HH_
# define INTERFACE_XML_INTERNAL_HH_

# include <string>
# include "com/centreon/broker/misc/unordered_hash.hh"
# include "mapping.hh"

namespace                   interface {
  namespace                 xml {
    template                <typename T>
    struct                  getter_setter {
      data_member<T> const* member;
      void                  (* getter)(T const&,
                              std::string const&,
                              data_member<T> const&,
                              std::map<std::string, std::string>&);
      void                  (* setter)(T&,
                              data_member<T> const&,
                              char const*);
    };

    // XML mappings.
    template                <typename T>
    struct                  xml_mapped_type {
      static umap<std::string, getter_setter<T> > map;
    };

    // Mapping initialization routine.
    void initialize();
  }
}

#endif /* !INTERFACE_XML_INTERNAL_HH_ */
