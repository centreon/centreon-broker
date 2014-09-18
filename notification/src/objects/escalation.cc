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

#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/escalation.hh"

using namespace com::centreon::broker::notification::objects;

const escalation::name_to_action escalation::_service_actions[] =
{{"w", service_warning},
 {"u", service_unknown},
 {"c", service_critical},
 {"r", service_recovery}};

const escalation::name_to_action escalation::_host_actions[] =
{{"d", host_down},
 {"u", host_unreachable},
 {"r", host_recovery}};

escalation::escalation() :
  _type(unknown),
  _escalation_options(none),
  _first_notification(0),
  _last_notification(0),
  _notification_interval(0) {

}

escalation::escalation(escalation const& obj) {
  escalation::operator=(obj);
}

escalation& escalation::operator=(escalation const& obj) {
  if (this != &obj) {
    _type = obj._type;
    _escalation_options = obj._escalation_options;
    _escalation_period = obj._escalation_period;
    _first_notification = obj._first_notification;
    _last_notification = obj._last_notification;
    _notification_interval = obj._notification_interval;
  }
  return (*this);
}

void escalation::set_types(type t) throw() {
  _type = t;
}

void escalation::set_type(type t) throw() {
  _type = (type)(_type | t);
}

bool escalation::is_type(type t) const throw() {
  return (_type & t);
}

escalation::action_on escalation::get_escalation_options() const throw() {
  return (_escalation_options);
}

void escalation::set_escalation_options(action_on val) throw() {
  _escalation_options = val;
}

void escalation::set_escalation_option(action_on val) throw() {
  _escalation_options = (action_on)(_escalation_options | val);
}

bool escalation::is_escalation_option_set(action_on val) const throw() {
  return (_escalation_options & val);
}

std::string const& escalation::get_escalation_period() const throw() {
  return (_escalation_period);
}

void escalation::set_escalation_period(const std::string& val) {
  _escalation_period = val;
}

unsigned int escalation::get_first_notification() const throw() {
  return (_first_notification);
}

void escalation::set_first_notification(unsigned int val) throw() {
  _first_notification = val;
}

unsigned int escalation::get_last_notification() const throw() {
  return (_last_notification);
}

void escalation::set_last_notification(unsigned int val) throw() {
  _last_notification = val;
}

unsigned int escalation::get_notification_interval() const throw() {
  return (_notification_interval);
}

void escalation::set_notification_interval(unsigned int val) throw() {
  _notification_interval = val;
}

void escalation::parse_host_escalation_options(std::string const& line) {
  std::vector<std::string> tokens;
  string::split(line, tokens, ',');

  static int host_actions_size = sizeof(_host_actions) / sizeof(_host_actions[0]);
    for (std::vector<std::string>::const_iterator it(tokens.begin()),
         end(tokens.end()); it != end; ++it) {
      for (int i = 0; i < host_actions_size; ++i) {
        if (*it == _host_actions[i].name)
          set_escalation_option(_host_actions[i].action);
      }
    }
}

void  escalation::parse_service_escalation_options(std::string const& line) {
  std::vector<std::string> tokens;
  string::split(line, tokens, ',');

  static int service_actions_size = sizeof(_service_actions) / sizeof(_service_actions[0]);
    for (std::vector<std::string>::const_iterator it(tokens.begin()),
         end(tokens.end()); it != end; ++it) {
      for (int i = 0; i < service_actions_size; ++i) {
        if (*it == _service_actions[i].name)
          set_escalation_option(_service_actions[i].action);
      }
    }
}
