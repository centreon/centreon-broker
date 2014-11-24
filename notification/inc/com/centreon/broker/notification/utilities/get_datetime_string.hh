/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_UTILITIES_GET_DATETIME_STRING_HH
#  define CCB_NOTIFICATION_UTILITIES_GET_DATETIME_STRING_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace      notification {
  namespace    utilities {

    std::string  get_datetime_string(time_t raw_time,
                                     int max_length,
                                     int type,
                                     int format);

    enum date_type {
      long_date_time = 0,
      short_date_time = 1,
      short_date = 2,
      short_time = 3,
      http_date_time = 4
    };

    enum date_format {
      date_format_us = 0,
      date_format_euro = 1,
      date_format_iso8601 = 2,
      date_format_strict_iso8601
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_UTILITIES_GET_DATETIME_STRING_HH
