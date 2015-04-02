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
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_groups<true>(
                          macro_context const& context) {
  std::map<std::string, neb::host_group_member> const& group_map =
    context.get_cache().get_host(context.get_id()).get_groups();
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
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_groups<false>(
                          macro_context const& context) {
  std::map<std::string, neb::host_group_member> const& group_map =
    context.get_cache().get_host(context.get_id()).get_groups();
  if (!group_map.empty())
    return (group_map.begin()->first);
  else
    return ("");
}

/**
 *  @brief Get the groups of a service.
 *
 *  Specialization for all the groups were required.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_service_groups<true>(
                          macro_context const& context) {
  std::map<std::string, neb::service_group_member> const& group_map =
    context.get_cache().get_service(context.get_id())
                       .get_groups();
  std::string result;
  for (std::map<std::string, neb::service_group_member>::const_iterator
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
 *  @brief Get the groups of a service.
 *
 *  Specialization for only one group was required.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_service_groups<false>(
                          macro_context const& context) {
  std::map<std::string, neb::service_group_member> const& group_map =
    context.get_cache().get_service(context.get_id())
                       .get_groups();
  if (!group_map.empty())
    return (group_map.begin()->first);
  else
    return ("");
}

/**
 *  @brief Get the output of a host.
 *
 *  Specialization for short output.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_output<false>(
                          macro_context const& context) {
  std::string output =
    context.get_cache().get_host(
      context.get_id()).get_status().output.toStdString();
  return (output.substr(0, output.find_first_of('\n')));
}

/**
 *  @brief Get the output of a host.
 *
 *  Specialization for long output.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_host_output<true>(
                          macro_context const& context) {
  std::string output =
    context.get_cache().get_host(
      context.get_id()).get_status().output.toStdString();
  size_t found = output.find_first_of('\n');
  if (found != std::string::npos)
    return (output.substr(found + 1, std::string::npos));
  return ("");
}

/**
 *  @brief Get the output of a service.
 *
 *  Specialization for short output.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_service_output<false>(
                          macro_context const& context) {
  std::string output =
    context.get_cache().get_service(
      context.get_id()).get_status().output.toStdString();
  return (output.substr(0, output.find_first_of('\n')));
}

/**
 *  @brief Get the output of a service.
 *
 *  Specialization for long output.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <> std::string get_service_output<true>(
                          macro_context const& context) {
  std::string output =
    context.get_cache().get_service(
      context.get_id()).get_status().output.toStdString();
  size_t found = output.find_first_of('\n');
  if (found != std::string::npos)
    return (output.substr(found + 1, std::string::npos));
  return ("");
}

/**
 *  Get the state of a host.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_host_state(
              macro_context const& context) {
  short state =
    context.get_cache().get_host(
      context.get_id()).get_status().current_state;
  if (state == objects::node_state::host_up)
    return ("UP");
  else if (state == objects::node_state::host_down)
    return ("DOWN");
  else
    return ("UNREACHABLE");
}

/**
 *  Get the state of a service.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_service_state(
              macro_context const& context) {
  short state =
    context.get_cache().get_service(
      context.get_id()).get_status().current_state;
  if (state == objects::node_state::service_ok)
    return ("OK");
  else if (state == objects::node_state::service_warning)
    return ("WARNING");
  else if (state == objects::node_state::service_critical)
    return ("CRITICAL");
  else
    return ("UNKNOWN");
}


/**
 *  Get the last state of a host.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_last_host_state(
              macro_context const& context) {
  short state =
    context.get_cache().get_host(
      context.get_id()).get_status().current_state;
  if (state == objects::node_state::host_up)
    return ("UP");
  else if (state == objects::node_state::host_down)
    return ("DOWN");
  else
    return ("UNREACHABLE");
}

/**
 *  Get the last state of a service.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_last_service_state(
              macro_context const& context) {
  short state =
    context.get_cache().get_service(
      context.get_id()).get_prev_status().current_state;
  if (state == objects::node_state::service_ok)
    return ("OK");
  else if (state == objects::node_state::service_warning)
    return ("WARNING");
  else if (state == objects::node_state::service_critical)
    return ("CRITICAL");
  else
    return ("UNKNOWN");
}

/**
 *  Get the state type of a host.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_host_state_type(
              macro_context const& context) {
  if (context.get_cache().get_host(
        context.get_id()).get_status().state_type == 1)
    return ("HARD");
  else
    return ("SOFT");
}

/**
 *  Ge the state type of service.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_service_state_type(
              macro_context const& context) {
  if (context.get_cache().get_service(
        context.get_id()).get_status().state_type == 1)
    return ("HARD");
  else
    return ("SOFT");
}

/**
 *  Null getter, returns nothing.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::null_getter(
              macro_context const& context) {
  (void)context;
  return ("");
}

/**
 *  Get the duration of a host state change.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_host_duration(
              macro_context const& context) {
  time_t last_state_change =
    context.get_cache().get_host(
      context.get_id()).get_status().last_state_change;
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
 *  Get the duration of a service state change.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_service_duration(
              macro_context const& context) {
  time_t last_state_change =
    context.get_cache().get_service(
      context.get_id()).get_status().last_state_change;
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
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_host_duration_sec(
             macro_context const& context) {
  time_t now(time(NULL));
  unsigned long duration(
    now - context.get_cache().get_host(
            context.get_id()).get_status().last_state_change);
  return (to_string<unsigned long, 0>(duration));
}

/**
 *  Get the duration of a service state change in seconds.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_service_duration_sec(
             macro_context const& context) {
  time_t now(time(NULL));
  unsigned long duration(
    now - context.get_cache().get_service(
            context.get_id()).get_status().last_state_change);
  return (to_string<unsigned long, 0>(duration));
}

/**
 *  Get the actual time in seconds.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_timet_string(
              macro_context const& context) {
  (void)context;
  return (to_string<time_t, 0>(::time(NULL)));
}

/**
 *  Get the type of a notification.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_notification_type(
              macro_context const& context) {
  action::action_type type = context.get_action().get_type();
  if (type == action::notification_attempt)
    return ("PROBLEM");
  else if (type == action::notification_up)
    return ("RECOVERY");
  else if (type == action::notification_ack)
    return ("ACKNOWLEDGEMENT");
  else if (type == action::notification_downtime)
    return ("DOWNTIME");
  else
    return ("UNKNOWN");
}
