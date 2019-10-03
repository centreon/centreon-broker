/*
** Copyright 2014-2015,2018 Centreon
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

#include "com/centreon/broker/notification/macro_getters.hh"

using namespace com::centreon::broker;

/**
 *  @brief Get the output of a host.
 *
 *  Specialization for short output.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
template <>
std::string notification::get_host_output<false>(macro_context const& context) {
  std::string output{
      context.get_cache().get_host(context.get_id()).get_status().output};
  return output.substr(0, output.find_first_of('\n'));
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
template <>
std::string notification::get_host_output<true>(macro_context const& context) {
  std::string output{
      context.get_cache().get_host(context.get_id()).get_status().output};
  size_t found = output.find_first_of('\n');
  if (found != std::string::npos)
    return output.substr(found + 1, std::string::npos);
  return "";
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
template <>
std::string notification::get_service_output<false>(
    macro_context const& context) {
  std::string output{
      context.get_cache().get_service(context.get_id()).get_status().output};
  return output.substr(0, output.find_first_of('\n'));
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
template <>
std::string notification::get_service_output<true>(
    macro_context const& context) {
  std::string output{
      context.get_cache().get_service(context.get_id()).get_status().output};
  size_t found = output.find_first_of('\n');
  if (found != std::string::npos)
    return output.substr(found + 1, std::string::npos);
  return "";
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
      context.get_cache().get_host(context.get_id()).get_status().current_state;
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
  short state = context.get_cache()
                    .get_service(context.get_id())
                    .get_status()
                    .current_state;
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
      context.get_cache().get_host(context.get_id()).get_status().current_state;
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
  short state = context.get_cache()
                    .get_service(context.get_id())
                    .get_prev_status()
                    .current_state;
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
  if (context.get_cache().get_host(context.get_id()).get_status().state_type ==
      1)
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
  if (context.get_cache()
          .get_service(context.get_id())
          .get_status()
          .state_type == 1)
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
  time_t last_state_change = context.get_cache()
                                 .get_host(context.get_id())
                                 .get_status()
                                 .last_state_change;
  // Get duration.
  time_t now(::time(NULL));
  unsigned long duration(now - last_state_change);

  // Break down duration.
  uint32_t days(duration / (24 * 60 * 60));
  duration %= (24 * 60 * 60);
  uint32_t hours(duration / (60 * 60));
  duration %= (60 * 60);
  uint32_t minutes(duration / 60);
  duration %= 60;

  // Stringify duration.
  std::ostringstream oss;
  oss << days << "d " << hours << "h " << minutes << "m " << duration << "s";
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
  time_t last_state_change = context.get_cache()
                                 .get_service(context.get_id())
                                 .get_status()
                                 .last_state_change;
  // Get duration.
  time_t now(::time(NULL));
  unsigned long duration(now - last_state_change);

  // Break down duration.
  uint32_t days(duration / (24 * 60 * 60));
  duration %= (24 * 60 * 60);
  uint32_t hours(duration / (60 * 60));
  duration %= (60 * 60);
  uint32_t minutes(duration / 60);
  duration %= 60;

  // Stringify duration.
  std::ostringstream oss;
  oss << days << "d " << hours << "h " << minutes << "m " << duration << "s";
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
  time_t now(::time(NULL));
  unsigned long duration(now - context.get_cache()
                                   .get_host(context.get_id())
                                   .get_status()
                                   .last_state_change);
  return (to_string<unsigned long, 0>(duration));
}

/**
 *  Get the number of downtime associated with a node.
 *
 *  @param[in] context  The context from where the macro is being executed.
 *
 *  @return  The value of the macro.
 */
std::string com::centreon::broker::notification::get_node_downtime_number(
    macro_context const& context) {
  unsigned long num = context.get_cache().node_downtimes(context.get_id());
  return (to_string<unsigned long, 0>(num));
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
  time_t now(::time(NULL));
  unsigned long duration(now - context.get_cache()
                                   .get_service(context.get_id())
                                   .get_status()
                                   .last_state_change);
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
