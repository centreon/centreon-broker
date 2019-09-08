/*
** Copyright 2009-2014 Centreon
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

#include "com/centreon/broker/notification/utilities/get_datetime_string.hh"
#include <cstdio>
#include <ctime>
#include <vector>

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::utilities;

/**
 *  given a date/time in time_t format, produce a corresponding
 *  date/time string, including timezone
 *
 *  @param[in] raw_time    The time to transform.
 *  @param[in] max_length  The max length of the resulting string.
 *  @param[in] type        The type.
 *
 *  @return                A string containing the date/time;
 */
std::string utilities::get_datetime_string(time_t raw_time,
                                           int max_length,
                                           int type,
                                           int format) {
  static char const* weekdays[7] = {"Sun", "Mon", "Tue", "Wed",
                                    "Thu", "Fri", "Sat"};
  static char const* months[12] = {"Jan", "Feb", "Mar",  "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

  std::vector<char> buffer;
  buffer.resize(max_length);

  std::string ret;
  ret.resize(max_length);

  tm tm_s;
  if (type == http_date_time)
    gmtime_r(&raw_time, &tm_s);
  else
    localtime_r(&raw_time, &tm_s);

  int hour(tm_s.tm_hour);
  int minute(tm_s.tm_min);
  int second(tm_s.tm_sec);
  int month(tm_s.tm_mon + 1);
  int day(tm_s.tm_mday);
  int year(tm_s.tm_year + 1900);

  char const* tzone(tm_s.tm_isdst ? tzname[1] : tzname[0]);

  /* ctime() style date/time */
  if (type == long_date_time)
    snprintf(&buffer[0], max_length, "%s %s %d %02d:%02d:%02d %s %d",
             weekdays[tm_s.tm_wday], months[tm_s.tm_mon], day, hour, minute,
             second, tzone, year);

  /* short date/time */
  else if (type == short_date_time) {
    if (format == date_format_euro)
      snprintf(&buffer[0], max_length, "%02d-%02d-%04d %02d:%02d:%02d", day,
               month, year, hour, minute, second);
    else if (format == date_format_iso8601 ||
             format == date_format_strict_iso8601)
      snprintf(&buffer[0], max_length, "%04d-%02d-%02d%c%02d:%02d:%02d", year,
               month, day, (format == date_format_strict_iso8601) ? 'T' : ' ',
               hour, minute, second);
    else
      snprintf(&buffer[0], max_length, "%02d-%02d-%04d %02d:%02d:%02d", month,
               day, year, hour, minute, second);
  }

  /* short date */
  else if (type == short_date) {
    if (format == date_format_euro)
      snprintf(&buffer[0], max_length, "%02d-%02d-%04d", day, month, year);
    else if (format == date_format_iso8601 ||
             format == date_format_strict_iso8601)
      snprintf(&buffer[0], max_length, "%04d-%02d-%02d", year, month, day);
    else
      snprintf(&buffer[0], max_length, "%02d-%02d-%04d", month, day, year);
  }

  /* expiration date/time for HTTP headers */
  else if (type == http_date_time)
    snprintf(&buffer[0], max_length, "%s, %02d %s %d %02d:%02d:%02d GMT",
             weekdays[tm_s.tm_wday], day, months[tm_s.tm_mon], year, hour,
             minute, second);

  /* short time */
  else
    snprintf(&buffer[0], max_length, "%02d:%02d:%02d", hour, minute, second);

  buffer[max_length - 1] = '\x0';

  return (std::string(&buffer[0]));
}
