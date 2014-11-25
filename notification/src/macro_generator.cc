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
#include <iomanip>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/macro_generator.hh"
#include "com/centreon/broker/notification/utilities/get_datetime_string.hh"
#include "com/centreon/broker/notification/macro_getters.hh"

using namespace com::centreon::broker::notification;

/**
 *  Default constructor.
 */
macro_generator::macro_generator() {
  if (_map.empty())
    _fill_x_macro_map(_map);
}

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
                        node_cache const& cache) const {
  objects::node::ptr node = st.get_node_by_id(id);
  if (!node)
    throw (exceptions::msg()
           << "notification: macro_generator: can't find the node: "
           << id.get_host_id() << ", " << id.get_service_id());
  objects::node::ptr host = node;
  if (id.is_service())
    host = st.get_node_by_id(objects::node_id(id.get_host_id()));
  if (!host)
    throw (exceptions::msg()
           << "notification: macro_generator: can't find the host "
           << id.get_host_id());
  node_cache::host_node_state const& hns = cache.get_host(id.get_host_id());

  for (macro_container::iterator it(container.begin()),
                                 end(container.end());
       it != end;
       ++it) {
    if (_get_global_macros(it.key(), st, *it))
      continue ;
    else if (_get_time_macros(it.key(), st.get_date_format(), *it))
      continue ;
    else if (_get_x_macros(it.key(), id, st, cache, *it))
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

bool macro_generator::_get_x_macros(
                        std::string const& macro_name,
                        objects::node_id id,
                        state const& st,
                        node_cache const& cache,
                        std::string& result) const {
  x_macro_map::const_iterator found = _map.find(macro_name);
  if (found == _map.end())
    return (false);
  else {
    result = (*found)(id, st, cache);
    return (true);
  }
}

/**
 *  Fill the macro generator map with functions used to generate macros.
 *
 *  @param[in] map  The map to fill.
 */
void macro_generator::_fill_x_macro_map(x_macro_map& map) {
  map.insert(
    "HOSTDISPLAYNAME",
    &get_host_member_as_string<
      neb::host_service,
      QString,
      &neb::host_service::display_name,
      0>);
  map.insert(
    "HOSTALIAS",
    &get_host_member_as_string<neb::host, QString, &neb::host::alias, 0>);
  map.insert(
    "HOSTADDRESS",
    &get_host_member_as_string<neb::host, QString, &neb::host::address, 0>);
  map.insert(
    "HOSTSTATE",
    &get_host_state);
  map.insert(
    "HOSTSTATEID",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_state,
      0>);
  map.insert(
    "LASTHOSTSTATE",
    &get_last_host_state);
  map.insert(
    "LASTHOSTSTATEID",
    &get_host_prevstatus_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_state,
      0>);
  map.insert(
    "HOSTSTATETYPE",
    &get_host_state_type);
  map.insert(
    "HOSTATTEMPT",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_check_attempt,
      0>);
  map.insert(
    "MAXHOSTATTEMPTS",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::max_check_attempts,
      0>);
  // Event and problem id macros are ignored.
  map.insert("HOSTEVENTID", &null_getter);
  map.insert("LASTHOSTEVENTID", &null_getter);
  map.insert("HOSTPROBLEMID", &null_getter);
  map.insert("LASTHOSTPROBLEMID", &null_getter);
  map.insert(
    "HOSTLATENCY",
    &get_host_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::latency,
      3>);
  map.insert(
    "HOSTEXECUTIONTIME",
    &get_host_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::execution_time,
      3>);
  map.insert(
    "HOSTDURATION",
    get_host_duration);
  map.insert(
    "HOSTDURATIONSEC",
    get_host_duration_sec);
  map.insert(
    "HOSTDOWNTIME",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::scheduled_downtime_depth,
      0>);
  map.insert(
    "HOSTPERCENTCHANGE",
    &get_host_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::percent_state_change,
      2>);
  map.insert(
    "HOSTGROUPNAME",
    &get_host_groups<false>);
  map.insert(
    "HOSTGROUPNAMES",
    &get_host_groups<true>);
  map.insert(
    "LASTHOSTCHECK",
    &get_host_status_member_as_string<
      neb::host_service_status,
      timestamp,
      &neb::host_service_status::last_check,
      0>);
  map.insert(
    "LASTHOSTSTATECHANGE",
    &get_host_status_member_as_string<
      neb::host_service_status,
      timestamp,
      &neb::host_service_status::last_state_change,
      0>);
  map.insert(
    "LASTHOSTUP",
    &get_host_status_member_as_string<
      neb::host_status,
      timestamp,
      &neb::host_status::last_time_up,
      0>);
  map.insert(
    "LASTHOSTDOWN",
    &get_host_status_member_as_string<
      neb::host_status,
      timestamp,
      &neb::host_status::last_time_down,
      0>);
  map.insert(
    "LASTHOSTUNREACHABLE",
    &get_host_status_member_as_string<
      neb::host_status,
      timestamp,
      &neb::host_status::last_time_unreachable,
      0>);
  map.insert(
    "HOSTOUTPUT",
    &get_host_output<false>);
  map.insert(
    "LONGHOSTOUTPUT",
    &get_host_output<true>);
  map.insert(
    "HOSTPERFDATA",
    &get_host_status_member_as_string<
      neb::host_service_status,
      QString,
      &neb::host_service_status::perf_data,
      0>);
  map.insert(
    "HOSTCHECKCOMMAND",
    &get_host_status_member_as_string<
      neb::host_service_status,
      QString,
      &neb::host_service_status::check_command,
      0>);
  // Hst ack macros are deprecated and ignored.
  map.insert("HOSTACKAUTHOR", &null_getter);
  map.insert("HOSTACKAUTHORNAME", &null_getter);
  map.insert("HOSTACKAUTHORALIAS", &null_getter);
  map.insert("HOSTACKCOMMENT", &null_getter);
}
