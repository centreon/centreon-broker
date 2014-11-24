/*
** Copyright 2011-2014 Merethis
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

#include <sstream>
#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/macro_generator.hh"
#include "com/centreon/broker/notification/utilities/get_datetime_string.hh"

using namespace com::centreon::broker::notification;

/**
 *  Generate macros.
 *
 *  @param[in,out] container  A container of asked macro, which will be filled as a result.
 *  @param[in] id             The id of the node for which we create macros.
 *  @param[in] cnt            The contact.
 *  @param[in] st             The state.
 *  @param[in] cache          The node cache.
 */
void macro_generator::generate(
                        macro_container& container,
                        objects::node_id id,
                        objects::contact const& cnt,
                        state const& st,
                        node_cache const& cache) {
  for (macro_container::iterator it(container.begin()),
                                 end(container.end());
       it != end;
       ++it) {
    if (_get_global_macros(it.key(), st, *it))
      continue ;
    else if (_get_time_macros(it.key(), st.get_date_format(), *it))
      continue ;
  }
}

/**
 *  Get this macro if it's a global macro.
 *
 *  @param[in] macro_name  The name of the macro.
 *  @param[in] st          The state.
 *  @param[out] result     The result, filled if the macro is global.
 *  @return                True if this macro was found in the global macros.
 */
bool macro_generator::_get_global_macros(
                        std::string const& macro_name,
                        state const& st,
                        std::string& result) {
  QHash<std::string, std::string> const& global_macros =
                                           st.get_global_macros();
  QHash<std::string, std::string>::const_iterator found =
    global_macros.find(macro_name);
  if (found == global_macros.end())
    return (false);
  result = *found;
  return (true);
}

/**
 *  Get this macro if it's a time macro.
 *
 *  @param[in] macro_name   The name of the macro.
 *  @param[in] date_format  The format of the date (ie US, Euro, Iso...)
 *  @param[out] result      The result, filled if the macro is a time macro.
 *
 *  @return                 True if the macro is a time macro.
 */
bool macro_generator::_get_time_macros(
                        std::string const& macro_name,
                        int date_format,
                        std::string& result) {
  time_t now = ::time(NULL);
  static const int max_string_length = 48;
  if (macro_name == "LONGDATETIME")
    result = utilities::get_datetime_string(
                          now,
                          max_string_length,
                          utilities::long_date_time,
                          date_format);
  else if (macro_name == "SHORTDATETIME")
    result = utilities::get_datetime_string(
                          now,
                          max_string_length,
                          utilities::short_date_time,
                          date_format);
  else if (macro_name == "DATE")
    result = utilities::get_datetime_string(
                          now,
                          max_string_length,
                          utilities::short_date,
                          date_format);
  else if (macro_name == "TIME")
    result = utilities::get_datetime_string(
                          now,
                          max_string_length,
                          utilities::short_time,
                          date_format);
  else if (macro_name == "TIMET") {
    std::stringstream ss;
    ss << now;
    result = ss.str();
  }
  else return (false);

  return (true);
}
