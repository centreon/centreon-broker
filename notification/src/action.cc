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

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

action::action()
  : _act(action::unknown),
    _id() {}

action::action(action const& obj) {
  action::operator=(obj);
}

action& action::operator=(action const& obj) {
  if (this != &obj) {
    _act = obj._act;
    _id = obj._id;
  }
  return (*this);
}

action::action_type action::get_type() const throw() {
  return (_act);
}

void action::set_type(action_type type) throw() {
  _act = type;
}

node_id action::get_node_id() const throw() {
  return (_id);
}

void action::set_node_id(objects::node_id id) throw() {
  _id = id;
}
