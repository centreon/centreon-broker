/*
** Copyright 2009-2015 Centreon
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

#include "com/centreon/broker/notification/action.hh"
#include "com/centreon/broker/notification/process_manager.hh"
#include "com/centreon/broker/notification/state.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;
using namespace com::centreon::broker::time;

/**
 *  Default constructor.
 */
action::action()
    : _act(action::unknown),
      _forwarded_action(action::unknown),
      _id(),
      _notification_rule_id(0),
      _notification_number(0),
      _at(0),
      _first_time_of_notification(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to be copied.
 */
action::action(action const& obj) {
  action::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to be copied.
 *
 *  @return         A reference to this object.
 */
action& action::operator=(action const& obj) {
  if (this != &obj) {
    _act = obj._act;
    _id = obj._id;
    _notification_rule_id = obj._notification_rule_id;
    _notification_number = obj._notification_number;
    _at = obj._at;
    _forwarded_action = obj._forwarded_action;
    _first_time_of_notification = obj._first_time_of_notification;
  }
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] obj  The object from which to test equality.
 *
 *  @return         True if the two objects are equal.
 */
bool action::operator==(action const& obj) const {
  return (_act == obj._act && _id == obj._id &&
          _notification_rule_id == obj._notification_rule_id &&
          _notification_number == obj._notification_number && _at == obj._at &&
          _forwarded_action == obj._forwarded_action &&
          _first_time_of_notification == obj._first_time_of_notification);
}

/**
 *  Comparison operator.
 *
 *  @param[in] obj  The compared object.
 *
 *  @return         True if this object is less than the other.
 */
bool action::operator<(action const& obj) const {
  if (_act != obj._act)
    return (_act < obj._act);
  else if (_id != obj._id)
    return (_id < obj._id);
  else if (_notification_rule_id != obj._notification_rule_id)
    return (_notification_rule_id < obj._notification_rule_id);
  else if (_notification_number != obj._notification_number)
    return (_notification_number < obj._notification_number);
  else if (_at != obj._at)
    return (_at < obj._at);
  else if (_first_time_of_notification != obj._first_time_of_notification)
    return (_first_time_of_notification < obj._first_time_of_notification);
  else
    return (_forwarded_action < obj._forwarded_action);
}

/**
 *  Get the type of this action.
 *
 *  @return[in]  The type of this action.
 */
action::action_type action::get_type() const throw() {
  return (_act);
}

/**
 *  Set the type of this action.
 *
 *  @param[in] type  The type of this action.
 */
void action::set_type(action_type type) throw() {
  _act = type;
}

/**
 *  Get the forwarded type.
 *
 *  @return  The forwarded type.
 */
action::action_type action::get_forwarded_type() const throw() {
  return (_forwarded_action);
}

/**
 *  Set the forwarded type.
 *
 *  @param[in] type  The forwarded type.
 */
void action::set_forwarded_type(action_type type) throw() {
  _forwarded_action = type;
}

/**
 *  Get the node id associated with this action.
 *
 *  @return  The node id associated with this action.
 */
node_id action::get_node_id() const throw() {
  return (_id);
}

/**
 *  Set the node id associated with this actions.
 *
 *  @param[in] id  The node id associated with this action.
 */
void action::set_node_id(objects::node_id id) throw() {
  _id = id;
}

/**
 *  Get the notification rule ID.
 *
 *  @return Notification rule ID.
 */
uint32_t action::get_notification_rule_id() const throw() {
  return (_notification_rule_id);
}

/**
 *  Set the notification rule ID.
 *
 *  @param[in] id  Notification rule ID.
 */
void action::set_notification_rule_id(uint32_t id) throw() {
  _notification_rule_id = id;
  return;
}

/**
 *  Get the notification number.
 *
 *  @return Current notification number.
 */
uint32_t action::get_notification_number() const throw() {
  return (_notification_number);
}

/**
 *  Set the notification number.
 *
 *  @param[in] num  Notification number.
 */
void action::set_notification_number(uint32_t num) throw() {
  _notification_number = num;
  return;
}

/**
 *  Get the scheduled time of this action.
 *
 *  @return  The scheduled time of this action.
 */
time_t action::get_at() const throw() {
  return (_at);
}

/**
 *  Set the scheduled time of this action.
 *
 *  @param[in] at  The scheduled time of this action.
 */
void action::set_at(time_t at) throw() {
  _at = at;
}

/**
 *  Get the first notification time.
 *
 *  @return  The first notification time.
 */
time_t action::get_first_notification_time() const throw() {
  return (_first_time_of_notification);
}

/**
 *  Set the first notification time.
 *
 *  @param[in] t  The new first notification time.
 */
void action::set_first_notification_time(time_t t) throw() {
  _first_time_of_notification = t;
}

/**
 *  @brief Process the action.
 *
 *  What is done changes based on the type of this notification.
 *
 *  @param[in] state            The notification state of the engine.
 *  @param[in] cache            The data cache of the module.
 *  @param[out] spawned_actions The actions to add to the queue after the
 * processing.
 *
 */
void action::process_action(
    state& st,
    node_cache& cache,
    std::vector<std::pair<time_t, action> >& spawned_actions) const {
  if (_act == unknown || _id == node_id())
    return;

  if (_act == notification_processing)
    _spawn_notification_attempts(st, spawned_actions);
  else
    _process_notification(st, cache, spawned_actions);
}

/**
 *  @brief Spawn the notification attempts from a notification processing.
 *
 *  A notification processing spawn one notification attempt for each rule
 *  associated to a particular node.
 *
 *  @param[in] st               The notification state of the engine.
 *  @param[out] spawned_actions The actions to add to the queue after the
 * processing.
 */
void action::_spawn_notification_attempts(
    state& st,
    std::vector<std::pair<time_t, action> >& spawned_actions) const {
  log_v2::notification()->debug(
      "notification: spawning notification action for node ({}, {})",
      _id.get_host_id(), _id.get_service_id());

  // Spawn an action for each rules.
  QList<notification_rule::ptr> rules = st.get_notification_rules_by_node(_id);
  for (QList<notification_rule::ptr>::iterator it(rules.begin()),
       end(rules.end());
       it != end; ++it) {
    // Build action (viability checks will be made later.
    action a;
    a.set_node_id(_id);
    a.set_type(_forwarded_action);
    a.set_notification_rule_id((*it)->get_id());
    a.set_notification_number(1);

    // Choose running time.
    time_t at;
    timeperiod::ptr tp = st.get_timeperiod_by_id((*it)->get_timeperiod_id());
    if (tp)
      at = tp->get_next_valid(::time(NULL));
    else
      at = ::time(NULL);
    spawned_actions.push_back(std::make_pair(at, a));
    spawned_actions.back().second.set_first_notification_time(at);
  }
  return;
}

/**
 *  Process a notification attempt.
 *
 *  @param[in] st                 The notification state of the engine.
 *  @param[in] cache              The data cache of the module.
 *  @param[out] spawned_actions   The action to add to the queue after the
 * processing.
 */
void action::_process_notification(
    state& st,
    node_cache& cache,
    std::vector<std::pair<time_t, action> >& spawned_actions) const {
  log_v2::notification()->debug(
      "notification: processing action for rule {} of node ({}, {})",
      _notification_rule_id, _id.get_host_id(), _id.get_service_id());

  // Check action viability.
  log_v2::notification()->debug(
      "notification: checking action viability for node ({}, {})",
      _id.get_host_id(), _id.get_service_id());

  // Check the node's existence.
  node::ptr n(st.get_node_by_id(_id));
  if (!n) {
    log_v2::notification()->debug(
        "notification: node ({}, {}) was not declared, notification attempt is "
        "not viable",
        _id.get_host_id(), _id.get_service_id());
    return;
  }

  // Get all the necessary data.
  notification_rule::ptr rule =
      st.get_notification_rule_by_id(_notification_rule_id);
  if (!rule) {
    log_v2::notification()->error(
        "notification: aborting notification attempt on node ({}, {}): rule {} "
        "does not exist",
        _id.get_host_id(), _id.get_service_id(), _notification_rule_id);
    return;
  }

  timeperiod::ptr tp = st.get_timeperiod_by_id(rule->get_timeperiod_id());
  if (!tp)
    log_v2::notification()->error(
        "notification: could not find timeperiod {} during notification "
        "attempt on node ({}, {}), so any time will be valid",
        rule->get_timeperiod_id(), _id.get_host_id(), _id.get_service_id());

  notification_method::ptr method =
      st.get_notification_method_by_id(rule->get_method_id());
  if (!method) {
    log_v2::notification()->error(
        "notification: aborting notification attempt on node ({}, {}): method "
        "{} does not exist",
        _id.get_host_id(), _id.get_service_id(), rule->get_method_id());
    return;
  }

  contact::ptr cnt = st.get_contact_by_id(rule->get_contact_id());
  if (!cnt) {
    log_v2::notification()->error(
        "notification: aborting notification attempt on node ({}, {}): contact "
        "{} does not exist",
        _id.get_host_id(), _id.get_service_id(), rule->get_contact_id());
    return;
  }

  command::ptr cmd = st.get_command_by_id(method->get_command_id());
  if (!cmd) {
    log_v2::notification()->error(
        "notification: aborting notification attempt on node ({}, {}): command "
        "{} does not exist",
        _id.get_host_id(), _id.get_service_id(), method->get_command_id());
    return;
  }

  // Check the existence of correlated parent.
  if (n->has_parent() && !method->should_be_notified_when_correlated()) {
    log_v2::notification()->debug(
        "notification: node ({}, {}) has parent issue, notification attempt is "
        "not viable",
        _id.get_host_id(), _id.get_service_id());
    return;
  }

  // Check if the state is valid.
  if (!method->should_be_notified_for(n->get_hard_state(),
                                      n->get_node_id().is_service())) {
    log_v2::notification()->debug(
        "notification: node ({}, {}) should not be notified for state {} "
        "according to method {}",
        _id.get_host_id(), _id.get_service_id(),
        static_cast<int>(n->get_hard_state()), rule->get_method_id());
    return;
  }

  // Check if the notification type is valid.
  if (!method->should_be_notified_for(_act)) {
    log_v2::notification()->debug(
        "notification: node ({}, {}) should not be notified for action type {} "
        "according to method {}",
        _id.get_host_id(), _id.get_service_id(), static_cast<int>(_act),
        rule->get_method_id());
    return;
  }

  // See if the timeperiod is valid.
  time_t now = ::time(NULL);
  if (tp && !tp->is_valid(now)) {
    log_v2::notification()->debug(
        "notification: notification attempt on node ({}, {}) is not in a valid "
        "timeperiod, rescheduling it at the next valid time",
        _id.get_host_id(), _id.get_service_id());
    spawned_actions.push_back(std::make_pair(tp->get_next_valid(now), *this));
    return;
  }

  bool should_send_the_notification = true;
  action next = *this;
  next.set_notification_number(_notification_number + 1);

  // See if the node is in downtime.
  if (_act == notification_attempt && cache.node_in_downtime(_id) == true) {
    log_v2::notification()->debug(
        "notification: node ({}, {}) is in downtime, notification won't be "
        "sent",
        _id.get_host_id(), _id.get_service_id());
    should_send_the_notification = false;
  }

  // See if the node has been acknowledged.
  if (_act == notification_attempt && cache.node_acknowledged(_id) == true) {
    log_v2::notification()->debug(
        "notification: node ({},{}) is acknowledged, notification won't be "
        "sent",
        _id.get_host_id(), _id.get_service_id());
    should_send_the_notification = false;
  }

  // See if this notification time is between the start and end.
  if (now < (_first_time_of_notification + method->get_start()) ||
      (now >= (_first_time_of_notification + method->get_end()) &&
       method->get_end())) {
    log_v2::notification()->debug(
        "notification: notification time of node ({}, {}) is '{}' and not in "
        "the method's valid range ([{}-{}])",
        _id.get_host_id(), _id.get_service_id(), now,
        _first_time_of_notification + method->get_start(),
        _first_time_of_notification + method->get_end());
    should_send_the_notification = false;
  }

  // Send the notification.
  if (should_send_the_notification) {
    std::string resolved_command = cmd->resolve(cnt, n, cache, st, *this);
    log_v2::notification()->info(
        "notification: launching notification command on node ({}, {}): {}",
        _id.get_host_id(), _id.get_service_id(), resolved_command);
    process_manager& manager = process_manager::instance();
    manager.create_process(resolved_command);
  }

  // Create the next notification.
  if (_act == notification_attempt)
    spawned_actions.push_back(
        std::make_pair(now + method->get_interval(), next));
}
