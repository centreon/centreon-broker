/*
** Copyright 2009-2012 Centreon
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

#ifndef CCB_NDO_INTERNAL_HH
#  define CCB_NDO_INTERNAL_HH

#  include <sstream>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "mapping.hh"

CCB_BEGIN()

namespace                 ndo {
  template                <typename T>
  struct                  getter_setter {
    data_member<T> const* member;
    void                  (* getter)(
                            T const&,
                            data_member<T> const&,
                            std::stringstream& buffer);
    void                  (* setter)(
                            T&,
                            data_member<T> const&,
                            char const*);
  };

  // NDO mappings.
  template                <typename T>
  struct                  ndo_mapped_type {
    static umap<int, getter_setter<T> > map;
  };

  // Mapping initialization routine.
  void initialize();
}

CCB_END()

#endif // !CCB_NDO_INTERNAL_HH
