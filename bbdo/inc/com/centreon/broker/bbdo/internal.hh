/*
** Copyright 2013-2014 Centreon
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

#ifndef CCB_BBDO_INTERNAL_HH
#  define CCB_BBDO_INTERNAL_HH

#  include <QByteArray>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "mapping.hh"

#  define BBDO_HEADER_SIZE 8
#  define BBDO_VERSION_MAJOR 1
#  define BBDO_VERSION_MINOR 2
#  define BBDO_VERSION_PATCH 0

#  define BBDO_NEB_TYPE 1u
#  define BBDO_STORAGE_TYPE 2u
#  define BBDO_CORRELATION_TYPE 3u
#  define BBDO_BAM_TYPE 4u
#  define BBDO_DUMPER_TYPE 5u
#  define BBDO_INTERNAL_TYPE 65535u
#  define BBDO_ID(type, id) ((type << 16) | id)

CCB_BEGIN()

namespace                 bbdo {
  // Data elements.
  enum {
    de_version_response = 1
  };

  template                <typename T>
  struct                  getter_setter {
    data_member<T> const* member;
    void (*               getter)(
                            T const&,
                            data_member<T> const&,
                            QByteArray&);
    unsigned int (*       setter)(
                            T&,
                            data_member<T> const&,
                            void const*,
                            unsigned int);
  };

  // BBDO mappings.
  template                <typename T>
  struct                  bbdo_mapped_type {
    static std::vector<getter_setter<T> > table;
  };

  // Mapping initialization routin.e
  void initialize();
}

CCB_END()

#endif // !CCB_BBDO_INTERNAL_HH
