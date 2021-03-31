/*
** Copyright 2011-2013,2017 Centreon
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

#include "com/centreon/broker/storage/parser.hh"

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "com/centreon/broker/database/table_max_size.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker::storage;

/**************************************
 *                                     *
 *            Local Objects            *
 *                                     *
 **************************************/

/**
 *  Extract a real value from a perfdata string.
 *
 *  @param[in,out] str  Pointer to a perfdata string.
 *  @param[in]     skip true to skip semicolon.
 *
 *  @return Extracted real value if successful, NaN otherwise.
 */
static inline double extract_double(char const** str, bool skip = true) {
  double retval;
  char* tmp;
  if (isspace(**str))
    retval = NAN;
  else {
    char const* comma{strchr(*str, ',')};
    if (comma) {
      /* In case of comma decimal separator, we duplicate the number and
       * replace the comma by a point. */
      size_t t = strcspn(comma, " \t\n\r;");
      char* nb = strndup(*str, (comma - *str) + t);
      nb[comma - *str] = '.';
      retval = strtod(nb, &tmp);
      if (nb == tmp)
        retval = NAN;
      *str = *str + (tmp - nb);
      free(nb);
    } else {
      retval = strtod(*str, &tmp);
      if (*str == tmp)
        retval = NAN;
      *str = tmp;
    }
    if (skip && (**str == ';'))
      ++*str;
  }
  return retval;
}

/**
 *  Extract a range from a perfdata string.
 *
 *  @param[out]    low       Low threshold value.
 *  @param[out]    high      High threshold value.
 *  @param[out]    inclusive true if range is inclusive, false
 *                           otherwise.
 *  @param[in,out] str       Pointer to a perfdata string.
 */
static inline void extract_range(double* low,
                                 double* high,
                                 bool* inclusive,
                                 char const** str) {
  // Exclusive range ?
  if ((**str) == '@') {
    *inclusive = true;
    ++*str;
  } else
    *inclusive = false;

  // Low threshold value.
  double low_value;
  if ('~' == **str) {
    low_value = -INFINITY;
    ++*str;
  } else
    low_value = extract_double(str);

  // High threshold value.
  double high_value;
  if (**str != ':') {
    high_value = low_value;
    if (!std::isnan(low_value))
      low_value = 0.0;
  } else {
    ++*str;
    char const* ptr(*str);
    high_value = extract_double(str);
    if (std::isnan(high_value) && ((*str == ptr) || (*str == (ptr + 1))))
      high_value = INFINITY;
  }

  // Set values.
  *low = low_value;
  *high = high_value;
}

/**
 *  Parse perfdata string as given by plugin.
 *
 *  @param[in]  str Raw perfdata string.
 *  @param[out] pd  List of parsed metrics.
 */
void parser::parse_perfdata(uint32_t host_id,
                            uint32_t service_id,
                            const char* str,
                            std::list<perfdata>& pd) {
  auto id = [host_id, service_id] {
    if (host_id || service_id)
      return fmt::format("({}:{})", host_id, service_id);
    else
      return std::string();
  };

  size_t start = strspn(str, " \n\r\t");
  const char* buf = str + start;

  // Debug message.
  log_v2::perfdata()->debug("storage: parsing service {} perfdata string '{}'",
                            id(), buf);

  char const* tmp = buf;

  auto skip = [](char const* tmp) -> char const* {
    while (*tmp && !isspace(*tmp))
      ++tmp;
    while (isspace(*tmp))
      ++tmp;
    return tmp;
  };

  while (*tmp) {
    bool error = false;

    // Perfdata object.
    perfdata p;

    // Get metric name.
    bool in_quote{false};
    char const* end{tmp};
    while (*end && (in_quote || (*end != '=' && !isspace(*end)) ||
                    static_cast<unsigned char>(*end) >= 128)) {
      if ('\'' == *end)
        in_quote = !in_quote;
      ++end;
    }

    /* The metric name is in the range s[0;size) */
    char const* s{tmp};
    tmp = end;
    --end;

    // Unquote metric name. Just beginning quotes and ending quotes"'".
    // We also remove spaces by the way.
    if (*s == '\'')
      ++s;
    if (*end == '\'')
      --end;

    while (*s && strchr(" \n\r\t", *s))
      ++s;
    while (end != s && strchr(" \n\r\t", *end))
      --end;

    /* The label is given by s and finishes at end */
    if (*end == ']') {
      --end;
      if (strncmp(s, "a[", 2) == 0) {
        s += 2;
        p.value_type(perfdata::absolute);
      } else if (strncmp(s, "c[", 2) == 0) {
        s += 2;
        p.value_type(perfdata::counter);
      } else if (strncmp(s, "d[", 2) == 0) {
        s += 2;
        p.value_type(perfdata::derive);
      } else if (strncmp(s, "g[", 2) == 0) {
        s += 2;
        p.value_type(perfdata::gauge);
      }
    }

    if (end - s + 1 > 0) {
      std::string name(std::move(std::string(s, end - s + 1)));
      name.resize(misc::string::adjust_size_utf8(
          name, get_metrics_col_size(metrics_metric_name)));
      p.name(std::move(name));
    } else {
      log_v2::perfdata()->error(
          "In service {}, metric name empty before '{}...'", id(),
          fmt::string_view(s, 10));
      error = true;
    }

    // Check format.
    if (*tmp != '=') {
      int i;
      for (i = 0; i < 10 && tmp[i]; i++)
        ;
      log_v2::perfdata()->error(
          "invalid perfdata format in service {}: equal sign not present or "
          "misplaced '{}'",
          id(), fmt::string_view(s, (tmp - s) + i));
      error = true;
    } else
      ++tmp;

    if (error) {
      tmp = skip(tmp);
      continue;
    }

    // Extract value.
    p.value(extract_double(const_cast<char const**>(&tmp), false));
    if (std::isnan(p.value())) {
      int i;
      for (i = 0; i < 10 && tmp[i]; i++)
        ;

      log_v2::perfdata()->error(
          "storage: invalid perfdata format in service {}: no numeric value "
          "after equal sign "
          "'{}'",
          id(), fmt::string_view(s, (tmp - s) + i));
      error = true;
      tmp = skip(tmp);
      continue;
    }

    // Extract unit.
    size_t t = strcspn(tmp, " \t\n\r;");
    {
      std::string unit(std::move(std::string(tmp, t)));
      unit.resize(misc::string::adjust_size_utf8(
          unit, get_metrics_col_size(metrics_unit_name)));
      p.unit(std::move(unit));
    }
    tmp += t;
    if (*tmp == ';')
      ++tmp;

    // Extract warning.
    {
      double warning_high;
      double warning_low;
      bool warning_mode;
      extract_range(&warning_low, &warning_high, &warning_mode,
                    const_cast<char const**>(&tmp));
      p.warning(warning_high);
      p.warning_low(warning_low);
      p.warning_mode(warning_mode);
    }

    // Extract critical.
    {
      double critical_high;
      double critical_low;
      bool critical_mode;
      extract_range(&critical_low, &critical_high, &critical_mode,
                    const_cast<const char**>(&tmp));
      p.critical(critical_high);
      p.critical_low(critical_low);
      p.critical_mode(critical_mode);
    }

    // Extract minimum.
    p.min(extract_double(const_cast<const char**>(&tmp)));

    // Extract maximum.
    p.max(extract_double(const_cast<const char**>(&tmp)));

    // Log new perfdata.
    log_v2::perfdata()->debug(
        "storage: got new perfdata (name={}, value={}, unit={}, warning={}, "
        "critical={}, min={}, max={})",
        p.name(), p.value(), p.unit(), p.warning(), p.critical(), p.min(),
        p.max());

    // Append to list.
    pd.emplace_back(std::move(p));

    // Skip whitespaces.
    while (isspace(*tmp))
      ++tmp;
  }
}
