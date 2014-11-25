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

#include "com/centreon/broker/notification/macro_getters.hh"

using namespace com::centreon::broker::notification;

/**
 *  @brief Get the groups of a host.
 *
 *  Specialization for all the groups were required.
 *
 *  @param[in] id     The id of the host.
 *  @param[in] st     The state of the conf.
 *  @param[in] cache  A node cache.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_groups<true>(
                          objects::node_id id,
                          state const& st,
                          node_cache const& cache) {
  std::map<std::string, neb::host_group_member> const& group_map =
    cache.get_host(id.get_host_id()).get_groups();
  std::string result;
  for (std::map<std::string, neb::host_group_member>::const_iterator
         it(group_map.begin()),
         end(group_map.end());
       it != end;
       ++it) {
    if (!result.empty())
      result.append(", ");
    result.append(it->first);
  }
  return (result);
}

/**
 *  @brief Get the groups of a host.
 *
 *  Specialization for only one group was required.
 *
 *  @param[in] id     The id of the host.
 *  @param[in] st     The state of the conf.
 *  @param[in] cache  A node cache.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_groups<false>(
                          objects::node_id id,
                          state const& st,
                          node_cache const& cache) {
  std::map<std::string, neb::host_group_member> const& group_map =
    cache.get_host(id.get_host_id()).get_groups();
  if (!group_map.empty())
    return (group_map.begin()->first);
}

/**
 *  @brief Get the output of a host.
 *
 *  Specialization for short output.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_output<false>(
                          objects::node_id id,
                          state const& st,
                          node_cache const& cache) {
  std::string output = cache.get_host(
                         id.get_host_id()).get_status().output.toStdString();
  return (output.substr(0, output.find_first_of('\n')));
}

/**
 *  @brief Get the output of a host.
 *
 *  Specialization for long output.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_output<true>(
                          objects::node_id id,
                          state const& st,
                          node_cache const& cache) {
  std::string output = cache.get_host(
                         id.get_host_id()).get_status().output.toStdString();
  size_t found = output.find_first_of('\n');
  if (found != std::string::npos)
    return (output.substr(found == std::string::npos));
  return ("");
}

/**
 *  Get the state of a host.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
std::string get_host_state(
              objects::node_id id,
              state const& st,
              node_cache const& cache) {
  node_cache::host_node_state const& node_state =
                                       cache.get_host(id.get_host_id());
  if (node_state.get_status().current_state == 0)
    return ("UP");
  else if (node_state.get_status().current_state == 1)
    return ("DOWN");
  else
    return ("UNREACHABLE");
}

/**
 *  Get the last state of a host.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
std::string get_last_host_state(
              objects::node_id id,
              state const& st,
              node_cache const& cache) {
  node_cache::host_node_state const& node_state =
                                       cache.get_host(id.get_host_id());
  if (node_state.get_prev_status().current_state == 0)
    return ("UP");
  else if (node_state.get_prev_status().current_state == 1)
    return ("DOWN");
  else
    return ("UNREACHABLE");
}

/**
 *  Get the state type of a host.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
std::string get_host_state_type(
              objects::node_id id,
              state const& st,
              node_cache const& cache) {
  if (cache.get_host(id.get_host_id()).get_status().state_type == 1)
    return ("HARD");
  else
    return ("SOFT");
}

/**
 *  Null getter, returns nothing.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
std::string null_getter(objects::node_id id,
                        state const& st,
                        node_cache const& cache) {
  (void)id;
  (void)st;
  (void)cache;
  return ("");
}

/**
 *  Get the duration of a host state change.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 */
std::string get_host_duration(
              objects::node_id id,
              state const& st,
              node_cache const& cache) {
  time_t last_state_change =
            cache.get_host(id.get_host_id()).get_status().last_state_change;
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

/**
 *  Get the duration of a host state change in seconds.
 *
 *  @param[in] id            The id of the host.
 *  @param[in] st            The state of the conf.
 *  @param[in] cache         A node cache.
 *
 *  @return  The value of the macro.
 */
std::string get_host_duration_sec(
             objects::node_id id,
             state const& st,
             node_cache const& cache) {
  time_t now(time(NULL));
  unsigned long duration(
    now - cache.get_host(id.get_host_id()).get_status().last_state_change);
  return (to_string<unsigned long, 0>(duration));
}
