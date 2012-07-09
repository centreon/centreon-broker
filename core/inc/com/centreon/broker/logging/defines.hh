/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_LOGGING_DEFINES_HH
#  define CCB_LOGGING_DEFINES_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace logging {
  // Log levels.
  enum    level {
    none = 0,
    high,
    medium,
    low,
    NONE = none,
    HIGH = high,
    MEDIUM = medium,
    LOW = low
  };

  // Log types.
  enum    type {
    config_type = 1,
    debug_type = 2,
    error_type = 4,
    info_type = 8
  };
}

CCB_END()

#endif // !CCB_LOGGING_DEFINES_HH
