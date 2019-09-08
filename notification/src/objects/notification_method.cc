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

#include "com/centreon/broker/notification/objects/notification_method.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
notification_method::notification_method()
    : _command_id(0), _interval(0), _start(0), _end(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
notification_method::notification_method(notification_method const& obj) {
  notification_method::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
notification_method& notification_method::operator=(
    notification_method const& obj) {
  if (this != &obj) {
    _name = obj._name;
    _command_id = obj._command_id;
    _interval = obj._interval;
    _status = obj._status;
    _types = obj._types;
    _start = obj._start;
    _end = obj._end;
  }
  return (*this);
}

/**
 *  Get the name of this notification method.
 *
 *  @return  The name of this notification method.
 */
std::string const& notification_method::get_name() const throw() {
  return (_name);
}

/**
 *  Set the name of this notification method.
 *
 *  @param[in] val  The new name of this notification method.
 */
void notification_method::set_name(std::string const& val) {
  _name = val;
}

/**
 *  Get the id of the command associated with this method.
 *
 *  @return  The id of the command associated with this method.
 */
unsigned int notification_method::get_command_id() const throw() {
  return (_command_id);
}

/**
 *  Set the id of the command associated with this method.
 *
 *  @param[in] id  The new id of the command associated with this method.
 */
void notification_method::set_command_id(unsigned int id) throw() {
  _command_id = id;
}

/**
 *  Get the interval of this notification method.
 *
 *  @return  The interval of this notification method.
 */
unsigned int notification_method::get_interval() const throw() {
  return (_interval);
}

/**
 *  Set the interval of this notification method.
 *
 *  @param[in] val  The new interval of this notification method.
 */
void notification_method::set_interval(unsigned int val) throw() {
  _interval = val;
}

/**
 *  Get the status when this method should be executed.
 *
 *  @return  The status when this method should be executed.
 */
std::string const& notification_method::get_status() const throw() {
  return (_status);
}

/**
 *  Set the status when this method should be executed.
 *
 *  @param[in] val  The new status when this method should be executed.
 */
void notification_method::set_status(std::string const& val) {
  _status = val;
}

/**
 *  Get the types when this method should be executed.
 *
 *  @return  The types when this method should be executed.
 */
std::string const& notification_method::get_types() const throw() {
  return (_types);
}

/**
 *  Set the types when this method should be executed.
 *
 *  @param[in] val  The new types when this method should be executed.
 */
void notification_method::set_types(std::string const& val) {
  _types = val;
}

/**
 *  Get the start of the escalation range of this method.
 *
 *  @return  The start of the escalation range of this method.
 */
unsigned int notification_method::get_start() const throw() {
  return (_start);
}

/**
 *  Set the start of the escalation range of this method.
 *
 *  @param[in] val  The new start of the escalation range of this method.
 */
void notification_method::set_start(unsigned int val) throw() {
  _start = val;
}

/**
 *  Get the end of the escalation range of this method.
 *
 *  @return  The end of the escalation range of this method.
 */
unsigned int notification_method::get_end() const throw() {
  return (_end);
}

/**
 *  Set the end of the escalation range of this method.
 *
 *  @param[in] val  The new end of the escalation range of this method.
 */
void notification_method::set_end(unsigned int val) throw() {
  _end = val;
}

/**
 *  Check if the status allow a notification.
 *
 *  @param[in] state        State of the node.
 *  @param[in] is_service   Is the node a service?
 *
 *  @return                 True if the notification is allowed.
 */
bool notification_method::should_be_notified_for(node_state state,
                                                 bool is_service) const {
  if (state == node_state::ok)
    return (_status.find_first_of('o') != std::string::npos);
  if (is_service) {
    if (state == node_state::service_warning)
      return (_status.find_first_of('w') != std::string::npos);
    else if (state == node_state::service_critical)
      return (_status.find_first_of('c') != std::string::npos);
    else if (state == node_state::service_unknown)
      return (_status.find_first_of('u') != std::string::npos);
  } else {
    if (state == node_state::host_down)
      return (_status.find_first_of('d') != std::string::npos);
    else if (state == node_state::host_unreachable)
      return (_status.find_first_of('n') != std::string::npos);
  }
  return (false);
}

/**
 *  Check if the types allow a notification.
 *
 *  @param[in] type  The type of the notification.
 *
 *  @return          True if the notification is allowed.
 */
bool notification_method::should_be_notified_for(
    action::action_type type) const {
  if (type == action::notification_attempt)
    return (_types.find_first_of('n') != std::string::npos);
  else if (type == action::notification_up)
    return (_types.find_first_of('r') != std::string::npos);
  else if (type == action::notification_ack)
    return (_types.find_first_of('a') != std::string::npos);
  else if (type == action::notification_downtime)
    return (_types.find_first_of('d') != std::string::npos);
  else
    return (false);
}

/**
 *  Check if the notification is valid even if the node is correlated.
 *
 *  @return  True if the notification is still valid for a correlated node.
 */
bool notification_method::should_be_notified_when_correlated() const {
  return (_types.find_first_of('c') != std::string::npos);
}
