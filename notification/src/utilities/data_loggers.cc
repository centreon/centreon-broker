/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/notification/utilities/data_loggers.hh"
#define LOG_MEMBER(klass, getter) \
  << #klass "::" #getter "() : " << (obj.getter()) << "; "
#define LOG_MEMBER_AS(klass, getter, cast) \
  << #klass "::" #getter "() : " << ((cast)obj.getter()) << "; "

using namespace com::centreon::broker;

/**
 *  Stream operator overload for node_id.
 *
 *  @param left The logging stream.
 *  @param obj The object to log.
 *  @return The logging stream.
 */
logging::temp_logger& logging::operator<<(
    logging::temp_logger const& left,
    notification::objects::node_id const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(node_id, get_host_id) LOG_MEMBER(node_id, get_service_id);
  return (tmp);
}

/**
 *  Stream operator overload for command.
 *
 *  @param left The logging stream.
 *  @param obj The object to log.
 *  @return The logging stream.
 */
logging::temp_logger& logging::operator<<(
    logging::temp_logger const& left,
    notification::objects::command const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(command, get_name);
  return (tmp);
}

/**
 *  Stream operator overload for dependency.
 *
 *  @param left The logging stream.
 *  @param obj The object to log.
 *  @return The logging stream.
 */
logging::temp_logger& logging::operator<<(
    logging::temp_logger const& left,
    notification::objects::dependency const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(dependency, is_host_dependency)
      LOG_MEMBER(dependency, is_service_dependency)
          LOG_MEMBER(dependency, get_period) LOG_MEMBER(dependency, get_kind)
              LOG_MEMBER(dependency, get_execution_failure_options)
                  LOG_MEMBER(dependency, get_inherits_parent)
                      LOG_MEMBER(dependency, get_notification_failure_options);
  return (tmp);
}

/**
 *  Stream operator overload for contact.
 *
 *  @param left The logging stream.
 *  @param obj The object to log.
 *  @return The logging stream.
 */
logging::temp_logger& logging::operator<<(
    logging::temp_logger const& left,
    notification::objects::contact const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(contact, get_description);
  return (tmp);
}

/**
 *  Stream operator overload for node_timeperiod.
 *
 *  @param left The logging stream.
 *  @param obj The object to log.
 *  @return The logging stream.
 */
logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          time::timeperiod const& obj) throw() {
  // TODO: Exceptions, timeranges.
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(timeperiod, get_alias) LOG_MEMBER(timeperiod, get_name)
      LOG_MEMBER(timeperiod, get_timezone);
  return (tmp);
}

/**
 *  Stream operator overload for node.
 *
 *  @param left The logging stream.
 *  @param obj The object to log.
 *  @return The logging stream.
 */
logging::temp_logger& logging::operator<<(
    logging::temp_logger const& left,
    notification::objects::node const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(node, get_notification_number) LOG_MEMBER(
      node, get_last_notification_time) LOG_MEMBER_AS(node, get_hard_state, int)
      LOG_MEMBER_AS(node, get_soft_state, int) LOG_MEMBER(node, get_node_id);
  return (tmp);
}

logging::temp_logger& logging::operator<<(
    temp_logger const& left,
    notification::objects::notification_method const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(notification_method, get_name)
      LOG_MEMBER(notification_method, get_command_id)
          LOG_MEMBER(notification_method, get_interval)
              LOG_MEMBER(notification_method, get_status)
                  LOG_MEMBER(notification_method, get_types)
                      LOG_MEMBER(notification_method, get_start)
                          LOG_MEMBER(notification_method, get_end);
  return (tmp);
}

logging::temp_logger& logging::operator<<(
    temp_logger const& left,
    notification::objects::notification_rule const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp LOG_MEMBER(notification_rule, get_method_id)
      LOG_MEMBER(notification_rule, get_timeperiod_id)
          LOG_MEMBER(notification_rule, get_contact_id)
              LOG_MEMBER(notification_rule, get_node_id);
  return (tmp);
}
