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

node::node() :
  _notification_number(0),
  _notifications_enabled(true),
  _notification_interval(0),
  _last_notification_time(0),
  _hard_state(0),
  _soft_state(0) {}

node::node(node const& obj) {
  node::operator=(obj);
}

node& node::operator=(node const& obj) {
  if (this != &obj) {
    _notification_number = obj._notification_number;
    _notifications_enabled = obj._notifications_enabled;
    _notification_interval = obj._notification_interval;
    _last_notification_time = obj._last_notification_time;
    _hard_state = obj._hard_state;
    _soft_state = obj._soft_state;
    _parents = obj._parents;
    _notification_period = obj._notification_period;
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

std::set<node_id> const& node::get_parents() const throw() {
  return (_parents);
}

void node::add_parent(node_id id) {
  _parents.insert(id);
}

void node::remove_parent(node_id id) {
  std::set<node_id>::iterator it(_parents.find(id));
  if (it != _parents.end())
    _parents.erase(it);
}

bool node::has_parent() const throw() {
  return (!_parents.empty());
}

bool node::get_notifications_enabled() const throw() {
  return (_notifications_enabled);
}

void node::set_notifications_enabled(bool enable) throw() {
  _notifications_enabled = enable;
}

timeperiod const& node::get_notification_timeperiod() const throw() {
  return (_notification_period);
}

void node::set_notification_timeperiod(timeperiod const& tp) {
  _notification_period = tp;
}

double node::get_notification_interval() const throw() {
  return (_notification_interval);
}

void node::set_notification_interval(double val) throw() {
  _notification_interval = val;
}
