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

#  include "com/centreon/broker/namespace.hh"

#  define BBDO_HEADER_SIZE 8
#  define BBDO_VERSION_MAJOR 2
#  define BBDO_VERSION_MINOR 0
#  define BBDO_VERSION_PATCH 0
#  define BBDO_INTERNAL_TYPE 65535u

CCB_BEGIN()

namespace                 bbdo {
  // Data elements.
  enum {
    de_version_response = 1
  };

  // Load/unload of BBDO protocol.
  void                    load();
  void                    unload();
}

CCB_END()

#endif // !CCB_BBDO_INTERNAL_HH
