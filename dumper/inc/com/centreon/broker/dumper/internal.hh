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

#ifndef CCB_DUMPER_INTERNAL_HH
#  define CCB_DUMPER_INTERNAL_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace dumper {
  // Data elements.
  enum data_element {
    de_dump = 1,
    de_timestamp_cache,
    de_remove,
    de_reload,
    de_db_dump,
    de_entries_ba,
    de_entries_kpi
  };
}

CCB_END()

#endif // !CCB_DUMPER_INTERNAL_HH
