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

#include "com/centreon/broker/notification/action.hh"
#include "com/centreon/broker/notification/state.hh"

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

  switch (_act) {
  case notification_attempt:
    return (_process_notification(st));
    break;
  default:
    break;
  }
  return (false);
}

bool action::_process_notification(state& st) {
  return_value node_viability = _check_notification_node_viability(st);
  if (node_viability == error_should_remove)
    return (false);
  else if (node_viability == error_should_reschedule)
    return (true);

  // Get the contact list attached to this node.
  QList<objects::contact::ptr> contacts =
    st.get_contacts_by_node(_id);

  // Iterate the list and get the viability.
  for (QList<objects::contact::ptr>::iterator it(contacts.begin()),
         end(contacts.end());
       it != end;
       ++it) {
    return_value contact_viability = _check_notification_contact_viability(**it, st);
    if (contact_viability == ok)
      _notify_contact_of_node(**it, st);
  }

  return false;
  /*else
    return (_check_notification_contact_viability(st));*/
}

action::return_value action::_check_notification_node_viability(state& st) {
  // Get current time.
  time_t current_time = time(NULL);

  // Find the node this notification is associated with.
  node::ptr n = st.get_node_by_id(_id);
  if (!n)
    return (error_should_remove);

  // If the node has no notification period and is a service, inherit one from the host
  timeperiod::ptr tp =
      st.get_timeperiod_by_name(n->get_notification_timeperiod());
  if (!tp) {
    if (_id.has_host())
      return (error_should_remove);
    node::ptr host = st.get_host_from_service(_id);
    if (!host)
      return (error_should_remove);
    tp = st.get_timeperiod_by_name(host->get_notification_timeperiod());
    if (!tp)
      return (error_should_remove);
  }

  // See if the node can have notifications sent out at this time
  if (tp->is_valid(current_time))
    return (error_should_reschedule);

  // Are notifications temporarily disabled for this node?
  if (!n->get_notifications_enabled())
    return (error_should_reschedule);

  // See if we should notify problems for this service.
  if (n->should_be_notified())
    return (error_should_remove);

  return (ok);
}

action::return_value action::_check_notification_contact_viability(
                      contact& con,
                      state& st) {
  // Get current time.
  time_t current_time = time(NULL);

  // Are notifications enabled for this contact?
  if ((_id.has_host() && !con.get_host_notifications_enabled()) ||
      (_id.has_service() && !con.get_service_notifications_enabled()))
    return (error_should_remove);

  // See if the contact can be notified at this time
  std::string notification_period = _id.has_service() ?
                                      con.get_service_notification_period() :
                                      con.get_host_notification_period();
  timeperiod::ptr tp = st.get_timeperiod_by_name(notification_period);
  if (!tp)
    return (error_should_remove);
  if (tp->is_valid(current_time))
    return (error_should_reschedule);

  // See if we should notify about problems with this service
  if (!con.can_be_notified(st.get_node_by_id(_id)->get_hard_state(),
                           !_id.has_service()))
    return (error_should_remove);

  return (ok);
}

void action::_notify_contact_of_node(objects::contact& con,
                                     state& st) {
  //Qlist<command::ptr> commands = _id.has_host() ? st.get_host_commands_by_contact(cnt)
  /*group commands = _id.has_host() ? con.get_host_notification_command() :
                                    con.get_service_notification_command();

  for*/
}
