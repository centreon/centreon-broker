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
 *  @param[in] state  The notification state of the.
 */
void action::process_action(state& st) {
  // Don't do anything if the action is an empty one.
  if (_act == unknown || _id == node_id())
    return;

  switch (_act) {
  case notification_attempt:
    _process_notification(st);
    break;
  default:
    break;
  }
}

void action::_process_notification(state& st) {

}

bool action::_check_notification_viability(::com::centreon::broker::notification::state& st) {
  // Get current time.
  time_t current_time;
  time(&current_time);

  // Find the node this notification is associated with.
  node::ptr n = st.get_node_by_id(_id);
  if (!n)
    return false;

  // If the node has no notification period and is a service, inherit one from the host
  timeperiod::ptr tp =
      st.get_timeperiod_by_name(n->get_notification_timeperiod());
  if (!tp) {
    if (_id.has_host())
      return false;
    node::ptr host = st.get_host_from_service(_id);
    if (!host)
      return false;
    tp = st.get_timeperiod_by_name(host->get_notification_timeperiod());
    if (!tp)
      return false;
  }

  // See if the node can have notifications sent out at this time

}
