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

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/parser.hh"
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
static inline double extract_double(
                       char const** str,
                       bool skip = true) {
  double retval;
  char* tmp;
  if (isspace(**str))
    retval = NAN;
  else {
    retval = strtod(*str, &tmp);
    if (*str == tmp)
      retval = NAN;
    *str = tmp;
    if (skip && (**str == ';'))
      ++*str;
  }
  return (retval);
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
static inline void extract_range(
                     double* low,
                     double* high,
                     bool* inclusive,
                     char const** str) {
  // Exclusive range ?
  if ((**str) == '@') {
    *inclusive = true;
    ++*str;
  }
  else
    *inclusive = false;

  // Low threshold value.
  double low_value;
  if ('~' == **str) {
    low_value = -INFINITY;
    ++*str;
  }
  else
    low_value = extract_double(str);

  // High threshold value.
  double high_value;
  if (**str != ':') {
    high_value = low_value;
    if (!isnan(low_value))
      low_value = 0.0;
  }
  else {
    ++*str;
    char const* ptr(*str);
    high_value = extract_double(str);
    if (isnan(high_value)
        && ((*str == ptr) || (*str == (ptr + 1))))
      high_value = INFINITY;
  }

  // Set values.
  *low = low_value;
  *high = high_value;

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
parser::parser() {}

/**
 *  @brief Copy constructor.
 *
 *  No data is stored within the parser class.
 *
 *  @param[in] pp Unused.
 */
parser::parser(parser const& pp) {
  (void)pp;
}

/**
 *  Destructor.
 */
parser::~parser() {}

/**
 *  @brief Assignment operator.
 *
 *  No data is stored within the parser class.
 *
 *  @param[in] pp Unused.
 *
 *  @return This object.
 */
parser& parser::operator=(parser const& pp) {
  (void)pp;
  return (*this);
}

/**
 *  Parse perfdata string as given by plugin.
 *
 *  @param[in]  str Raw perfdata string.
 *  @param[out] pd  List of parsed metrics.
 */
void parser::parse_perfdata(
               QString const& str,
               QList<perfdata>& pd) {
  // Extract metrics strings.
  std::string buf(str.toStdString());
  std::replace(buf.begin(), buf.end(), ',', '.');
  char const* ptr(buf.c_str());

  // Skip initial whitespaces.
  while (isblank(*ptr))
    ++ptr;
  while (*ptr) {
    // Debug message.
    logging::debug(logging::medium)
      << "storage: parsing perfdata string '" << ptr << "'";

    // Perfdata object.
    perfdata p;

    // Get metric name.
    bool in_quote(false);
    unsigned int i(0);
    while (ptr[i] && (in_quote || (ptr[i] != '='))) {
      if ('\'' == ptr[i])
        in_quote = !in_quote;
      ++i;
    }
    std::string s(ptr, i);
    ptr += i;

    // Unquote metric name.
    size_t t;
    t = s.find_first_of('\'');
    while (t != std::string::npos) {
      s.erase(t, 1);
      t = s.find_first_of('\'', t + 1); // Skip one char, so '' becomes '.
    }
    if ((s.size() > 0) && (*s.rbegin() == ']')) {
      if (!s.compare(0, 2, "a[")) {
        s = s.substr(2, s.size() - 3);
        p.value_type(perfdata::absolute);
      }
      else if (!s.compare(0, 2, "c[")) {
        s = s.substr(2, s.size() - 3);
        p.value_type(perfdata::counter);
      }
      else if (!s.compare(0, 2, "d[")) {
        s = s.substr(2, s.size() - 3);
        p.value_type(perfdata::derive);
      }
      else if (!s.compare(0, 2, "g[")) {
        s = s.substr(2, s.size() - 3);
        p.value_type(perfdata::gauge);
      }
    }
    p.name(QString(s.c_str()).trimmed());

    // Check format.
    if (*ptr != '=')
      throw (exceptions::perfdata() << "storage: invalid perfdata " \
               "format: equal sign not present or misplaced");
    ++ptr;

    // Extract value.
    p.value(extract_double(&ptr, false));
    if (isnan(p.value()))
      p.value(0.0);

    // Extract unit.
    t = strcspn(ptr, " ;");
    p.unit(std::string(ptr, t).c_str());
    ptr += t;
    if (*ptr == ';')
      ++ptr;

    // Extract warning.
    {
      double warning_high;
      double warning_low;
      bool warning_mode;
      extract_range(&warning_low, &warning_high, &warning_mode, &ptr);
      p.warning(warning_high);
      p.warning_low(warning_low);
      p.warning_mode(warning_mode);
    }

    // Extract critical.
    {
      double critical_high;
      double critical_low;
      bool critical_mode;
      extract_range(
        &critical_low,
        &critical_high,
        &critical_mode,
        &ptr);
      p.critical(critical_high);
      p.critical_low(critical_low);
      p.critical_mode(critical_mode);
    }

    // Extract minimum.
    p.min(extract_double(&ptr));

    // Extract maximum.
    p.max(extract_double(&ptr));

    // Log new perfdata.
    logging::debug(logging::low) << "storage: got new perfdata (name="
      << p.name() << ", value=" << p.value() << ", unit=" << p.unit()
      << ", warning=" << p.warning() << ", critical=" << p.critical()
      << ", min=" << p.min() << ", max=" << p.max() << ")";

    // Append to list.
    pd.push_back(p);

    // Skip whitespaces.
    while (isblank(*ptr))
      ++ptr;
  }

  return ;
}
