/*
** Copyright 2009-2011 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
