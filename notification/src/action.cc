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
    _notification_rule_id(0) {}

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
  }
  return (*this);
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
 *  Process the action.
 *
 *  @param[in] state            The notification state of the engine.
 *  @param[out] spawned_actions The action to add to the queue after the processing.
 *
 */
void action::process_action(
      state& st,
      std::vector<std::pair<time_t, action> >& spawned_actions) {
  if (_act == unknown || _id == node_id())
    return;

  if (_act == notification_processing)
    _spawn_notification_attempts(st, spawned_actions);
  else if (_act == notification_attempt)
    _process_notification(st, spawned_actions);
}

void action::_spawn_notification_attempts(
               ::com::centreon::broker::notification::state& st,
               std::vector<std::pair<time_t, action> >& spawned_actions) {
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
    // TODO: Compatibility check.

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

bool action::_check_action_viability(
              ::com::centreon::broker::notification::state& st) {
  logging::debug(logging::low)
      << "Notification: Checking action viability for node (host id = "
      << _id.get_host_id() << ", service_id = " << _id.get_service_id()
      << ").";

  node::ptr n;
  // Check the node's existence.
  if (!(n = st.get_node_by_id(_id)))
    return (false);

  // Check the existence of correlated parent.
  if (n->has_parent())
    return (false);

  return (true);
}

void action::_process_notification(state& st,
                                   std::vector<std::pair<time_t, action> >& spawned_actions) {

  logging::debug(logging::low)
      << "Notification: Processing notification action for notification_rule (host id = "
      << _id.get_host_id() << ", service_id = " << _id.get_service_id()
      << ", notification_rule_id = " << _notification_rule_id << ").";

  // Check action viability.
  if (!_check_action_viability(st))
    return;

  // Get all the necessary data.
  notification_rule::ptr rule = st.get_notification_rule_by_id(_notification_rule_id);
  if (!rule)
    return;

  timeperiod::ptr tp = st.get_timeperiod_by_id(rule->get_timeperiod_id());
  if (!tp)
    return;

  notification_method::ptr method = st.get_notification_method_by_id(rule->get_method_id());
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
  // TODO

  // Send the notification.
  std::string resolved_command /*= cmd.resolve()*/;
  process_manager& manager = process_manager::instance();

  manager.create_process(resolved_command);

}

action::return_value action::_check_notification_node_viability(state& st) {
  // Get current time.
  time_t current_time = time(NULL);

  // Find the node this notification is associated with.
  node::ptr n = st.get_node_by_id(_id);
  if (!n) {
    logging::debug(logging::low)
      << "Notification: Error: Could not find the node for this notification.";
    return (error_should_remove);
  }

  // If the node has a parent, don't send anything.
  if (n->has_parent()) {
    logging::debug(logging::low)
      << "Notification: This node has a correlated parent. Don't notify.";
    return (error_should_remove);
  }

  // Get the notification rules.
  QList<notification_rule::ptr> rules = st.get_notification_rules_by_node(_id);

  // If no rules, don't do anything.
  if (rules.empty())
    return (error_should_remove);

  // If the node has no notification period and is a service, inherit one from the host
  /*timeperiod::ptr tp =
      st.get_timeperiod_by_name(n->get_notification_timeperiod());
  if (!tp) {
    if (_id.is_host()) {
      logging::debug(logging::low)
        << "Notification: Error: Could not find the timeperiod for this "
           "notification.";
      return (error_should_remove);
    }
    node::ptr host = st.get_host_from_service(_id);
    if (!host) {
      logging::debug(logging::low)
        << "Notification: Error: Could not find the host for this "
           "service in notification.";
      return (error_should_remove);
    }
    tp = st.get_timeperiod_by_name(host->get_notification_timeperiod());
    if (!tp) {
      logging::debug(logging::low)
        << "Notification: Error: Could not find the timeperiod for this"
           " notification.";
      return (error_should_remove);
    }
  }

  // See if the node can have notifications sent out at this time
  if (tp->is_valid(current_time)) {
    logging::debug(logging::low)
      << "Notification: The notification shouldn't be sent at this time.";
    return (error_should_reschedule);
  }

  // Are notifications temporarily disabled for this node?
  if (!n->get_notifications_enabled()) {
    logging::debug(logging::low)
      << "Notification: Notification are temporarily disabled for this node.";
    return (error_should_reschedule);
  }

  // See if we should notify problems for this node.
  if (n->should_be_notified()) {
    logging::debug(logging::low)
      << "Notification: This node should not be notified for this state.";
    return (error_should_remove);
  }

  // See if the node is in downtime.
  if (st.is_node_in_downtime(_id) == true) {
    logging::debug(logging::low)
      << "Notification: This node is in downtime: don't send anything.";
    return (error_should_reschedule);
  }

  // See if the node has been acknowledged.
  if (st.has_node_been_acknowledged(_id) == true) {
    logging::debug(logging::low)
      << "Notification: This node has been acknowledged: don't send anything.";
    return (error_should_remove);
  }*/

  return (ok);
}

action::return_value action::_check_notification_contact_viability(
                      contact::ptr con,
                      state& st) {
  logging::debug(logging::low)
      << "Notification: Processing notification contact " << con->get_name()
      << ".";
  // Get current time.
  time_t current_time = time(NULL);

  // Are notifications enabled for this contact?
  if ((_id.is_host() && !con->get_host_notifications_enabled()) ||
      (_id.is_service() && !con->get_service_notifications_enabled())) {
    logging::debug(logging::low)
        << "Notification: Notification not enabled for this contact.";
    return (error_should_remove);
  }

  // See if the contact can be notified at this time
  std::string notification_period = _id.is_service() ?
                                      con->get_service_notification_period() :
                                      con->get_host_notification_period();
  /*timeperiod::ptr tp = st.get_timeperiod_by_name(notification_period);
  if (!tp) {
    logging::debug(logging::low)
        << "Notification: Error: Could not find timeperiod for this contact.";
    return (error_should_remove);
  }
  if (tp->is_valid(current_time)) {
    logging::debug(logging::low)
        << "Notification: The notification shouldn't be sent at this time for"
           " this contact.";
    return (error_should_reschedule);
  }

  // See if we should notify about problems with this service
  if (!con->can_be_notified(st.get_node_by_id(_id)->get_hard_state(),
                           !_id.is_service())) {
    logging::debug(logging::low)
        << "Notification: This contact should not be notified for this state.";
    return (error_should_remove);
  }*/

  return (ok);
}
