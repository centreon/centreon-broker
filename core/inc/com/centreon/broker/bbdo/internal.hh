/*
** Copyright 2013,2015 Merethis
**
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

#ifndef CCB_BBDO_INTERNAL_HH
#  define CCB_BBDO_INTERNAL_HH

#  include <QByteArray>
#  include <vector>
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/namespace.hh"

#  define BBDO_HEADER_SIZE 8
#  define BBDO_VERSION_MAJOR 1
#  define BBDO_VERSION_MINOR 2
#  define BBDO_VERSION_PATCH 0
#  define BBDO_INTERNAL_TYPE 65535u

CCB_BEGIN()

// Forward declarations.
namespace                 io {
  class                   data;
  class                   event_info;
}
namespace                 mapping {
  class                   entry;
}

namespace                 bbdo {
  // Data elements.
  enum {
    de_version_response = 1
  };

  struct                  getter_setter {
    void (*               getter)(
                            io::data const&,
                            mapping::entry const&,
                            QByteArray&);
    unsigned int (*       setter)(
                            io::data&,
                            mapping::entry const&,
                            void const*,
                            unsigned int);
  };

  // BBDO mapping.
  struct                       bbdo_mapped_type {
    io::event_info const*      mapped_type;
    std::vector<getter_setter> bbdo_entries;
  };
  extern umap<unsigned int, bbdo_mapped_type> bbdo_mapping;

  // Create mappings.
  void create_mappings();
  // Mapping load/unload routines.
  void load();
  void unload();
}

CCB_END()

#endif // !CCB_BBDO_INTERNAL_HH
