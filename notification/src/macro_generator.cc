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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/macro_generator.hh"

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
