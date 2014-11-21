/*
** Copyright 2009-2014 Merethis
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

#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/action.hh"
#include "com/centreon/broker/notification/state.hh"
#include "com/centreon/broker/notification/process_manager.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
action::action()
  : _act(action::unknown),
    _id(),
    _notification_rule_id(0),
    _notification_number(0),
    _at(0) {}

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
  return (_act == obj._act &&
          _id == obj._id &&
          _notification_rule_id == obj._notification_rule_id &&
          _notification_number == obj._notification_number &&
          _at == obj._at);
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
  else if(_id != obj._id)
    return (_id < obj._id);
  else if (_notification_rule_id != obj._notification_rule_id)
    return (_notification_rule_id < obj._notification_rule_id);
  else if (_notification_number == obj._notification_number)
    return (_notification_number < obj._notification_number);
  else
    return (_at < obj._at);
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
 *  @brief Process the action.
 *
 *  What is done changes based on the type of this notification.
 *
 *  @param[in] state            The notification state of the engine.
 *  @param[in] cache            The data cache of the module.
 *  @param[out] spawned_actions The actions to add to the queue after the processing.
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
  else if (_act == notification_attempt)
    _process_notification(st, cache, spawned_actions);
}

/**
 *  @brief Spawn the notification attempts from a notification processing.
 *
 *  A notification processing spawn one notification attempt for each rule
 *  associated to a particular node.
 *
 *  @param[in] st               The notification state of the engine.
 *  @param[out] spawned_actions The actions to add to the queue after the processing.
 */
void action::_spawn_notification_attempts(
               state& st,
               std::vector<std::pair<time_t, action> >& spawned_actions) const{
  logging::debug(logging::low)
      << "Notification: Spawning notification attempts for node (host id = "
      << _id.get_host_id() << ", service_id = " << _id.get_service_id()
      << ").";

  // If the action shouldn't be executed, do nothing.
  if (!_check_action_viability(st))
    return;

  node::ptr n = st.get_node_by_id(_id);
  // Spawn an action for each compatible rules.
  QList<notification_rule::ptr> rules = st.get_notification_rules_by_node(_id);
  for (QList<notification_rule::ptr>::iterator it(rules.begin()),
                                               end(rules.end());
       it != end;
       ++it) {

    if (!(*it)->should_be_notified_for(n->get_hard_state()))
      continue;

    action a;
    time_t at = time(NULL);

    a.set_node_id(_id);
    a.set_type(action::notification_attempt);
    a.set_notification_rule_id((*it)->get_id());
    timeperiod::ptr tp = st.get_timeperiod_by_id((*it)->get_timeperiod_id());
    // If no timeperiod, don't spawn the action.
    if (!tp)
      continue;
    tp->get_next_valid(at);
    spawned_actions.push_back(std::make_pair(at, action()));
  }
}

/**
 *  Generic checks of action viability.
 *
 *  @param[in] st  The notification state of the engine.
 *
 *  @return        True if the checks were successful.
 */
bool action::_check_action_viability(
               ::com::centreon::broker::notification::state& st) const {
  logging::debug(logging::low)
      << "Notification: Checking action viability for node (host id = "
      << _id.get_host_id() << ", service_id = " << _id.get_service_id()
      << ").";

  node::ptr n;
  // Check the node's existence.
  if (!(n = st.get_node_by_id(_id)))
    return (false);

  // Check the existence of correlated parent.
  if (n->has_parent()
      && !(n->get_notification_options()
           & objects::node_notification_opt::not_correlated))
    return (false);

  return (true);
}

/**
 *  Process a notification attempt.
 *
 *  @param[in] st                 The notification state of the engine.
 *  @param[in] cache              The data cache of the module.
 *  @param[out] spawned_actions   The action to add to the queue after the processing.
 */
void action::_process_notification(
       state& st,
       node_cache& cache,
       std::vector<std::pair<time_t, action> >& spawned_actions) const {

  logging::debug(logging::low)
      << "Notification: Processing notification action for notification_rule ("
      << "host id = " << _id.get_host_id() << ", "
      << "service_id = " << _id.get_service_id() << ", "
      << "notification_rule_id = " << _notification_rule_id << ").";

  // Check action viability.
  if (!_check_action_viability(st))
    return;

  // Get all the necessary data.
  notification_rule::ptr rule =
      st.get_notification_rule_by_id(_notification_rule_id);
  if (!rule)
    return;

  timeperiod::ptr tp = st.get_timeperiod_by_id(rule->get_timeperiod_id());
  if (!tp)
    return;

  notification_method::ptr method =
      st.get_notification_method_by_id(rule->get_method_id());
  if (!method)
    return;

  contact::ptr cnt = st.get_contact_by_id(rule->get_contact_id());
  if (!cnt)
    return;

  command::ptr cmd = st.get_command_by_id(method->get_command_id());
  if (!cmd)
    return;

  node::ptr n = st.get_node_by_id(_id);

  // Check if the notification should be sent.
  // See if the state is valid.
  if(!rule->should_be_notified_for(n->get_hard_state()))
    return;

  // See if the timeperiod is valid
  time_t now = time(NULL);
  if (!tp->is_valid(now)) {
    logging::debug(logging::low)
      << "Notification: The timeperiod is not valid: "
         "reschedule it for the next valid time.";
    spawned_actions.push_back(std::make_pair(tp->get_next_valid(now), *this));
    return;
  }

  bool should_send_the_notification = true;
  action next = *this;

  // See if the node is in downtime.
  if (st.is_node_in_downtime(_id) == true) {
    logging::debug(logging::low)
      << "Notification: This node is in downtime: don't send anything.";
    should_send_the_notification = false;
    return;
  }

  // See if the node has been acknowledged.
  if (st.has_node_been_acknowledged(_id) == true) {
    logging::debug(logging::low)
      << "Notification: This node has been acknowledged: don't send anything.";
    should_send_the_notification = false;
    return;
  }

  // See if this notification is between the start and end.
  if (_notification_number < method->get_start()
      || _notification_number >= method->get_end()) {
    should_send_the_notification = false;
    next.set_notification_number(_notification_number + 1);
  }

  // Send the notification.
  if (should_send_the_notification) {
    std::string resolved_command = cmd->resolve(rule, tp, method, cnt, n, cache);
    process_manager& manager = process_manager::instance();
    manager.create_process(resolved_command);
  }

  // Create the next notification.
  spawned_actions.push_back(std::make_pair(now + method->get_interval(),
                                           next));
}
