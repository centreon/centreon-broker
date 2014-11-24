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
    else if (_get_host_macros(it.key(), *host, hns, *it))
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

bool macro_generator::_get_host_macros(
                        std::string const& macro_name,
                        objects::node const& host,
                        node_cache::host_node_state const& host_state,
                        std::string& result) {
  static QSet<std::string> filter;
  if (filter.empty()) {
    filter.insert("HOSTDISPLAYNAME");
    filter.insert("HOSTALIAS");
    filter.insert("HOSTADDRESS");
    filter.insert("HOSTSTATE");
    filter.insert("HOSTSTATEID");
    filter.insert("LASTHOSTSTATE");
    filter.insert("LASTHOSTSTATEID");
    filter.insert("HOSTSTATETYPE");
    filter.insert("HOSTATTEMPT");
    filter.insert("MAXHOSTATTEMPTS");
    filter.insert("HOSTEVENTID");
    filter.insert("LASTHOSTEVENTID");
    filter.insert("HOSTPROBLEMID");
    filter.insert("LASTHOSTPROBLEMID");
    filter.insert("HOSTLATENCY");
    filter.insert("HOSTEXECUTIONTIME");
    filter.insert("HOSTDURATION");
    filter.insert("HOSTDURATIONSEC");
    filter.insert("HOSTDOWNTIME");
    filter.insert("HOSTPERCENTCHANGE");
    filter.insert("HOSTGROUPNAME");
    filter.insert("HOSTGROUPNAMES");
    filter.insert("LASTHOSTCHECK");
    filter.insert("LASTHOSTSTATECHANGE");
    filter.insert("LASTHOSTUP");
    filter.insert("LASTHOSTDOWN");
    filter.insert("LASTHOSTUNREACHABLE");
    filter.insert("HOSTOUTPUT");
    filter.insert("LONGHOSTOUTPUT");
    filter.insert("HOSTPERFDATA");
    filter.insert("HOSTCHECKCOMMAND");
    filter.insert("HOSTACKAUTHOR");
    filter.insert("HOSTACKAUTHORNAME");
    filter.insert("HOSTACKAUTHORALIAS");
    filter.insert("HOSTACKCOMMENT");
  }
  if (!filter.contains(macro_name))
    return (false);

  if (macro_name == "HOSTDISPLAYNAME")
    result = host_state.get_node().display_name.toStdString();
  else if (macro_name == "HOSTALIAS")
    result = host_state.get_node().alias.toStdString();
  else if (macro_name == "HOSTADDRESS")
    result = host_state.get_node().address.toStdString();
  else if (macro_name == "HOSTSTATE") {
    if (host_state.get_status().current_state == 0)
      result = "UP";
    else if (host_state.get_status().current_state == 1)
      result = "DOWN";
    else
      result = "UNKNOWN";
  }
  else if (macro_name == "HOSTSTATEID")
  {
    std::stringstream ss;
    ss << host_state.get_status().current_state;
    result = ss.str();
  }
  else if (macro_name == "LASTHOSTSTATE") {
    if (host_state.get_prev_status().current_state == 0)
      result = "UP";
    else if (host_state.get_prev_status().current_state == 1)
      result = "DOWN";
    else
      result = "UNREACHABLE";
  }
  else if (macro_name == "LASTHOSTSTATEID") {
    std::stringstream ss;
    ss << host_state.get_prev_status().current_state;
    result = ss.str();
  }
  else if (macro_name == "HOSTSTATETYPE") {
    if (host_state.get_status().state_type == 1)
      result = "HARD";
    else
      result = "SOFT";
  }
  else if (macro_name == "HOSTATTEMPT") {
    std::stringstream ss;
    ss << host_state.get_status().current_check_attempt;
    result = ss.str();
  }
  else if (macro_name == "MAXHOSTATTEMPTS") {
    std::stringstream ss;
    ss << host_state.get_status().max_check_attempts;
    result = ss.str();
  }
  else if (macro_name == "HOSTEVENTID") {
    // IGNORED
    result = "";
  }
  else if (macro_name == "LASTHOSTEVENTID") {
    // IGNORED
    result = "";
  }
  else if (macro_name == "HOSTPROBLEMID") {
    // IGNORED
    result = "";
  }
  else if (macro_name == "LASTHOSTPROBLEMID") {
    // IGNORED
    result = "";
  }
  else if (macro_name == "HOSTLATENCY") {
    std::stringstream ss;
    ss << host_state.get_status().latency;
    result = ss.str();
  }
  else if (macro_name == "HOSTEXECUTIONTIME") {
    std::stringstream ss;
    ss << host_state.get_status().execution_time;
    result = ss.str();
  }
  else if (macro_name == "HOSTDURATION") {
    std::stringstream ss;
    ss << _compute_duration(host_state.get_status().last_state_change);
    result = ss.str();
  }
  else if (macro_name == "HOSTDURATIONSEC") {
    std::stringstream ss;
    ss << _compute_duration(
            ::time(NULL) - host_state.get_status().last_hard_state_change);
    result = ss.str();
  }
  else if (macro_name == "HOSTDOWNTIME") {
    std::stringstream ss;
    ss << host_state.get_status().scheduled_downtime_depth;
    result = ss.str();
  }
  else if (macro_name == "HOSTPERCENTCHANGE") {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << host_state.get_status().percent_state_change;
    result = ss.str();
  }
  else if (macro_name == "HOSTGROUPNAME") {
    std::map<std::string, neb::host_group_member> const& group_map = host_state.get_groups();
    if (!group_map.empty())
      result = group_map.begin()->first;
  }
  else if (macro_name == "HOSTGROUPNAMES") {
    std::map<std::string, neb::host_group_member> const& group_map = host_state.get_groups();
    result.clear();
    for (std::map<std::string, neb::host_group_member>::const_iterator it(group_map.begin()),
                                                                       end(group_map.end());
         it != end;
         ++it) {
      if (!result.empty())
        result.append(", ");
      result.append(it->first);
    }
  }
  else if (macro_name == "LASTHOSTCHECK") {
    std::stringstream ss;
    ss << host_state.get_status().last_check;
    result = ss.str();
  }
  else if (macro_name == "LASTHOSTSTATECHANGE") {
    std::stringstream ss;
    ss << host_state.get_status().last_state_change;
    result = ss.str();
  }
  else if (macro_name == "LASTHOSTUP") {
    std::stringstream ss;
    ss << host_state.get_status().last_time_up;
    result = ss.str();
  }
  else if (macro_name == "LASTHOSTDOWN") {
    std::stringstream ss;
    ss << host_state.get_status().last_time_down;
    result = ss.str();
  }
  else if (macro_name == "LASTHOSTUNREACHABLE") {
    std::stringstream ss;
    ss << host_state.get_status().last_time_unreachable;
    result = ss.str();
  }
  else if (macro_name == "HOSTOUTPUT") {
    std::string output = host_state.get_status().output.toStdString();
    result = output.substr(0, output.find_first_of('\n'));
  }
  else if (macro_name == "LONGHOSTOUTPUT") {
    std::string output = host_state.get_status().output.toStdString();
    size_t found = output.find_first_of('\n');
    if (found != std::string::npos)
      result = output.substr(found == std::string::npos);
  }
  else if (macro_name == "HOSTPERFDATA") {
    result = host_state.get_status().perf_data.toStdString();
  }
  else if (macro_name == "HOSTCHECKCOMMAND") {
    result = host_state.get_status().check_command.toStdString();
  }
  else if (macro_name == "HOSTACKAUTHOR") {
    // DEPRECATED
  }
  else if (macro_name == "HOSTACKAUTHORNAME") {
    // DEPRECATED
  }
  else if (macro_name == "HOSTACKAUTHORALIAS") {
    // DEPRECATED
  }
  else if (macro_name == "HOSTACKCOMMENT") {
    // DEPRECATED
  }
  return (true);
}

/**
 *  Compute the duration.
 *
 *  @param last_state_change  The time of the last state change.
 *  @return                   A string containing the duration.
 */
std::string macro_generator::_compute_duration(time_t last_state_change) {
  // Get duration.
  time_t now(time(NULL));
  unsigned long duration(now - last_state_change);

  // Break down duration.
  unsigned int days(duration / (24 * 60 * 60));
  duration %= (24 * 60 * 60);
  unsigned int hours(duration / (60 * 60));
  duration %= (60 * 60);
  unsigned int minutes(duration / 60);
  duration %= 60;

  // Stringify duration.
  std::ostringstream oss;
  oss << days << "d "
      << hours << "h "
      << minutes << "m "
      << duration << "s";
  return (oss.str());
}
