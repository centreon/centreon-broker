/*
** Copyright 2011 Merethis
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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "exceptions/basic.hh"
#include "storage/parser.hh"
#include "storage/perfdata.hh"

using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

/**
 *  Extract a real value from a perfdata string.
 *
 *  @param[in,out] str Pointer to a perfdata string.
 *
 *  @return Extracted real value if successful, NaN otherwise.
 */
static inline double extract_double(char const** str) {
  double retval;
  char* tmp;
  retval = strtod(*str, &tmp);
  *str = tmp;
  if (**str == ';')
    ++*str;
  return (retval);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any attempt to use the copy constructor will result in a call to
 *  abort().
 *
 *  @param[in] pp Unused.
 */
parser::parser(parser const& pp) {
  (void)pp;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any attempt to use the assignment operator will result in a call to
 *  abort().
 *
 *  @param[in] pp Unused.
 *
 *  @return This object.
 */
parser& parser::operator=(parser const& pp) {
  (void)pp;
  assert(false);
  abort();
  return (*this);
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
 *  Destructor.
 */
parser::~parser() {}

/**
 *  Parse perfdata string as given by plugin.
 *
 *  @param[in]  str Raw perfdata string.
 *  @param[out] pd  List of parsed metrics.
 */
void parser::parse_perfdata(QString const& str,
                                     std::list<perfdata>& pd) {
  /* Extract metrics strings. */
  std::string buf(str.toStdString());
  char const* ptr(buf.c_str());
  // Skip initial whitespaces.
  while (isblank(*ptr))
    ++ptr;
  while (*ptr) {
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
    p.name(s.c_str());

    // Check format.
    if (*ptr != '=')
      throw (exceptions::basic() << "invalid perfdata format: equal sign not present or misplaced");
    ++ptr;

    // Extract value.
    p.value(extract_double(&ptr));
    if (isnan(p.value()))
      p.value(0.0);

    // Extract unit.
    t = strcspn(ptr, ";");
    p.unit(std::string(ptr, t).c_str());
    ptr += t;
    if (*ptr)
      ++ptr;

    // Extract warning.
    p.warning(extract_double(&ptr));

    // Extract critical.
    p.critical(extract_double(&ptr));

    // Extract minimum.
    p.min(extract_double(&ptr));

    // Extract maximum.
    p.max(extract_double(&ptr));

    // Append to list.
    pd.push_back(p);

    // Skip whitespaces.
    while (isblank(*ptr))
      ++ptr;
  }

  return ;
}
