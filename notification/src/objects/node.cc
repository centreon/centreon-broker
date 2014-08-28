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

using namespace com::centreon::broker::notification;

node::node() :
  _notification_number(0),
  _last_notification_time(0),
  _hard_state(0),
  _soft_state(0) {}

node::node(node const& obj) :
  _notification_number(obj._notification_number),
  _last_notification_time(obj._last_notification_time),
  _hard_state(obj._hard_state),
  _soft_state(obj._soft_state) {}

node& node::operator=(node const& obj) {
  if (this != &obj) {
    _notification_number = obj._notification_number;
    _last_notification_time = obj._last_notification_time;
    _hard_state = obj._hard_state;
    _soft_state = obj._soft_state;
  }
  return *this;
}

int node::get_notification_number() const throw() {
  return (_notification_number);
}

void node::set_notification_number(int value) {
  _notification_number = value;
}

time_t node::get_last_notification_time() const throw() {
  return (_last_notification_time);
}

void node::set_last_notification_time(time_t value) {
  _last_notification_time = value;
}

short node::get_hard_state() const throw() {
  return (_hard_state);
}

void node::set_hard_state(short value) {
  _hard_state = value;
}

short node::get_soft_state() const throw() {
  return (_soft_state);
}

void node::set_soft_state(short value) {
  _soft_state = value;
}

node_id node::get_node_id() const throw() {
  return (_id);
}

void node::set_node_id(node_id id) throw() {
  _id = id;
}
