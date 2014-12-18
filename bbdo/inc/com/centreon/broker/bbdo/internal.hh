/*
** Copyright 2013-2014 Merethis
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
#  include "com/centreon/broker/namespace.hh"
#  include "mapping.hh"

#  define BBDO_HEADER_SIZE 8
#  define BBDO_VERSION_MAJOR 1
#  define BBDO_VERSION_MINOR 1
#  define BBDO_VERSION_PATCH 1

#  define BBDO_NEB_TYPE 1u
#  define BBDO_STORAGE_TYPE 2u
#  define BBDO_CORRELATION_TYPE 3u
#  define BBDO_BAM_TYPE 4u
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
