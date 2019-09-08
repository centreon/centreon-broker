/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_NOTIFICATION_UTILITIES_GET_DATETIME_STRING_HH
#define CCB_NOTIFICATION_UTILITIES_GET_DATETIME_STRING_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace notification {
namespace utilities {

std::string get_datetime_string(time_t raw_time,
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
}  // namespace utilities
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_UTILITIES_GET_DATETIME_STRING_HH
