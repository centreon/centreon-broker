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

#include "com/centreon/broker/notification/objects/notification_rule.hh"

using namespace com::centreon::broker::notification::objects;

notification_rule::notification_rule()
  : _id(0),
    _method_id(0),
    _timeperiod_id(0),
    _contact_id(0) {}

notification_rule::notification_rule(notification_rule const& obj) {
  notification_rule::operator=(obj);
}

notification_rule& notification_rule::operator=(notification_rule const& obj) {
  if (this != &obj) {
    _id = obj._id;
    _method_id = obj._method_id;
    _timeperiod_id = obj._timeperiod_id;
    _contact_id = obj._contact_id;
    _node_id = obj._node_id;
  }
  return (*this);
}

unsigned int notification_rule::get_id() {
  return (_id);
}

void notification_rule::set_id(unsigned int id) {
  _id = id;
}

unsigned int notification_rule::get_method_id() const throw() {
  return (_method_id);
}

void notification_rule::set_method_id(unsigned int val) throw() {
  _method_id = val;
}

unsigned int notification_rule::get_timeperiod_id() const throw() {
  return (_timeperiod_id);
}

void notification_rule::set_timeperiod_id(unsigned int val) throw() {
  _timeperiod_id = val;
}

unsigned int notification_rule::get_contact_id() const throw() {
  return (_contact_id);
}

void notification_rule::set_contact_id(unsigned int val) throw() {
  _contact_id = val;
}

node_id notification_rule::get_node_id() const throw() {
  return (_node_id);
}

void notification_rule::set_node_id(node_id val) throw() {
  _node_id = val;
}
