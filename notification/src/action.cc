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
    _id() {}

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
 *  @param[in] state  The notification state of the engine.
 *
 *  @return           True if the action should be rescheduled.
 */
bool action::process_action(state& st) {
  // Don't do anything if the action is an empty one.
  if (_act == unknown || _id == node_id())
    return (false);

  if (_act == notification_attempt)
    return (_process_notification(st));
  else
    return (false);
}

bool action::_process_notification(state& st) {

  logging::debug(logging::low)
      << "Notification: Processing notification action for node (host id = "
      << _id.get_host_id() << ", service_id = " << _id.get_service_id()
      << ").";

  return_value node_viability = _check_notification_node_viability(st);
  if (node_viability == error_should_remove)
    return (false);
  else if (node_viability == error_should_reschedule)
    return (true);

  // Get the contact list attached to this node.
  QList<objects::contact::ptr> contacts =
    st.get_contacts_by_node(_id);

  logging::debug(logging::low)
      << "Notification: Processing notification contacts for node (host id = "
      << _id.get_host_id() << ", service_id = " << _id.get_service_id()
      << ").";

  // Iterate the list and get the viability.
  for (QList<objects::contact::ptr>::iterator it(contacts.begin()),
                                              end(contacts.end());
       it != end;
       ++it) {
    return_value contact_viability =
      _check_notification_contact_viability(*it, st);
    if (contact_viability == ok)
      _notify_contact_of_node(*it, st);
  }

  return false;
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

  // If the node has no notification period and is a service, inherit one from the host
  timeperiod::ptr tp =
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
  }

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
  timeperiod::ptr tp = st.get_timeperiod_by_name(notification_period);
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
  }

  return (ok);
}

void action::_notify_contact_of_node(contact::ptr cnt,
                                     state& st) {
  // Get the process manager.
  process_manager& manager = process_manager::instance();

  // Iterate on the commands associated with this contact.
  QList<command::ptr> commands = _id.is_host() ?
        st.get_host_commands_by_contact(cnt) :
        st.get_service_commands_by_contact(cnt);

  for (QList<command::ptr>::iterator it(commands.begin()), end(commands.end());
       it != end; ++it) {
    // Process command.
    std::string command /* = it->resolve()*/;

    manager.create_process(command);
  }
}
