/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/objects/node.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
node::node() :
  _notification_number(0),
  _notifications_enabled(true),
  _notification_interval(0),
  _notification_options(node_notification_opt::none),
  _last_notification_time(0),
  _hard_state(),
  _soft_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to be copied.
 */
node::node(node const& obj) {
  node::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to be copied.
 *
 *  @return         A reference to this object.
 */
node& node::operator=(node const& obj) {
  if (this != &obj) {
    _notification_number = obj._notification_number;
    _notifications_enabled = obj._notifications_enabled;
    _notification_interval = obj._notification_interval;
    _notification_options = obj._notification_options;
    _last_notification_time = obj._last_notification_time;
    _hard_state = obj._hard_state;
    _soft_state = obj._soft_state;
    _parents = obj._parents;
    _notification_period = obj._notification_period;
  }
  return *this;
}

/**
 *  @brief Get the notification number of this node.
 *
 *  Used internally by the notification engine.
 *
 *  @return  The notification number.
 */
int node::get_notification_number() const throw() {
  return (_notification_number);
}

/**
 *  @brief Set the notification number of this node.
 *
 *  Used internally by the notification engine.
 *
 *  @param[in] value  The value to set.
 */
void node::set_notification_number(int value) {
  _notification_number = value;
}

/**
 *  Get the time of the last notification.
 *
 *  @return  The time of the last notification.
 */
time_t node::get_last_notification_time() const throw() {
  return (_last_notification_time);
}

/**
 *  Set the time of the last notification.
 *
 *  @param[in] value  The time of the last notification.
 */
void node::set_last_notification_time(time_t value) {
  _last_notification_time = value;
}

/**
 *  Get the hard state of this node.
 *
 *  @return  The hard state of this node.
 */
node_state node::get_hard_state() const throw() {
  return (_hard_state);
}

/**
 *  Set the hard state of this node.
 *
 *  @param[in] value  The new value for the hard state of this node.
 */
void node::set_hard_state(node_state value) {
  _hard_state = value;
}

/**
 *  Get the soft state of this node.
 *
 *  @return  The soft state of this node.
 */
node_state node::get_soft_state() const throw() {
  return (_soft_state);
}

/**
 *  Set the soft state of this node.
 *
 *  @param[în] value  The new value for the soft state of this node.
 */
void node::set_soft_state(node_state value) {
  _soft_state = value;
}

/**
 *  Get this node id.
 *
 *  @return  This node id.
 */
node_id node::get_node_id() const throw() {
  return (_id);
}

/**
 *  Set this node id.
 *
 *  @param[in] id  The new value of this node id.
 */
void node::set_node_id(node_id id) throw() {
  _id = id;
}

/**
  *  Get the correlated parents of this node.
  *
  *  @return  A set containing the correlated parents of this node.
  */
std::set<node_id> const& node::get_parents() const throw() {
  return (_parents);
}

/**
 *  Add a parent to the correlated parent set of this node.
 *
 *  @param[in] id  The id of the parent to add.
 */
void node::add_parent(node_id id) {
  _parents.insert(id);
}

/**
 *  Remove a parent to the correlated parent set of this node.
 *
 *  @param[in] id  The id of the parent to remove.
 */
void node::remove_parent(node_id id) {
  std::set<node_id>::iterator it(_parents.find(id));
  if (it != _parents.end())
    _parents.erase(it);
}

/**
 *  Return true if this node has at least a correlated parent.
 *
 *  @return  True if this node has at least a correlated parent.
 */
bool node::has_parent() const throw() {
  return (!_parents.empty());
}

/**
 *  Return true if the notifications are enabled for this node.
 *
 *  @return  True if the notifications are enabled for this node.
 */
bool node::get_notifications_enabled() const throw() {
  return (_notifications_enabled);
}

/**
 *  Set the notification enabled flag.
 *
 *  @param[in] enable  True if the notification are enabled for this node.
 */
void node::set_notifications_enabled(bool enable) throw() {
  _notifications_enabled = enable;
}

/**
 *  Get the notification timeperiod attached to this node.
 *
 *  @return  The notification timeperiod of this node.
 */
std::string const& node::get_notification_timeperiod() const throw() {
  return (_notification_period);
}

/**
 *  Set the notification timeperiod attached to this node.
 *
 *  @param[in] tp  The notification timeperiod attached to this node.
 */
void node::set_notification_timeperiod(std::string const& tp) {
  _notification_period = tp;
}

/**
 *  Get the notification interval.
 *
 *  @return  The notification interval.
 */
double node::get_notification_interval() const throw() {
  return (_notification_interval);
}

/**
 *  Set the notification interval.
 *
 *  @param[in] val  The new value for the notification interval.
 */
void node::set_notification_interval(double val) throw() {
  _notification_interval = val;
}

/**
 *  Get the notification options.
 *
 *  @return  The notification options.
 */
node_notification_opt node::get_notification_options() const throw() {
  return (_notification_options);
}

/**
 *  Set the notification options.
 *
 *  @param[in] val  The notification options of this node.
 */
void node::set_notification_options(node_notification_opt val) throw() {
  _notification_options = val;
}

/**
 *  Should this node be notified based on options and current state?
 *
 *  @return  True if the node should be notified.
 */
bool node::should_be_notified() const throw() {
  if (_hard_state == node_state::ok)
    return (false);

  if (_id.is_host()) {
    if(_hard_state == node_state::host_down &&
        _notification_options.check_for(node_notification_opt::host_down))
      return (true);
    else if(_hard_state == node_state::host_unreachable &&
              _notification_options.check_for(node_notification_opt::host_unreachable))
      return (true);
  }
  else if (_id.is_service()) {
    if (_hard_state == node_state::service_warning &&
          _notification_options.check_for(node_notification_opt::service_warning))
      return (true);
    else if (_hard_state == node_state::service_critical &&
               _notification_options.check_for(node_notification_opt::service_critical))
      return (true);
    else if (_hard_state == node_state::service_unknown &&
               _notification_options.check_for(node_notification_opt::service_unknown))
      return (true);
  }
  return (false);
}
