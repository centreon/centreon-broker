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

/**
 *  Default constructor.
 */
escalation::escalation() :
  _type(unknown),
  _escalation_options(none),
  _first_notification(0),
  _last_notification(0),
  _notification_interval(0) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
escalation::escalation(escalation const& obj) {
  escalation::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to be copy.
 *
 *  @return         A reference to this object.
 */
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

/**
 *  Set the types of the escalation.
 *
 *  @param[in] t  The types of the escalation.
 */
void escalation::set_types(type t) throw() {
  _type = t;
}

/**
 *  Add an unitary type flag to the escalation types.
 *
 *  @param[in] t  The type to add to the escalation types.
 */
void escalation::set_type(type t) throw() {
  _type = (type)(_type | t);
}

/**
 *  Check if the given type flag is set in the escalation types.
 *
 *  @param[in] t  The type flag to check.
 *
 *  @return       True if this type flag was set.
 */
bool escalation::is_type(type t) const throw() {
  return (_type & t);
}

/**
 *  Get the escalation options.
 *
 *  @return  The escalation options.
 */
escalation::action_on escalation::get_escalation_options() const throw() {
  return (_escalation_options);
}

/**
 *  Set the escalation options.
 *
 *  @param[in] val  The new escalation options.
 */
void escalation::set_escalation_options(action_on val) throw() {
  _escalation_options = val;
}

/**
 *  Add an unitary escalation option flag to the set.
 *
 *  @param[in] val  The unitary escalation option flag to set.
 */
void escalation::set_escalation_option(action_on val) throw() {
  _escalation_options = (action_on)(_escalation_options | val);
}

/**
 *  Check if an unitary escalation option flag is set.
 *
 *  @param[in] val  The unitary escalation option flag to check.
 *
 *  @return         True if the flag is set.
 */
bool escalation::is_escalation_option_set(action_on val) const throw() {
  return (_escalation_options & val);
}

/**
 *  Get the escalation period.
 *
 *  @return  The escalation period.
 */
std::string const& escalation::get_escalation_period() const throw() {
  return (_escalation_period);
}

/**
 *  Set the escalation period.
 *
 *  @param[in] val  The new escalation period.
 */
void escalation::set_escalation_period(const std::string& val) {
  _escalation_period = val;
}

/**
 *  Get the first notification value.
 *
 *  @return  The first notification value.
 */
unsigned int escalation::get_first_notification() const throw() {
  return (_first_notification);
}

/**
 *  Set the first notification value.
 *
 *  @param[in] val  The first notification value.
 */
void escalation::set_first_notification(unsigned int val) throw() {
  _first_notification = val;
}

/**
 *  Get the last notification value.
 *
 *  @return  The last notification value.
 */
unsigned int escalation::get_last_notification() const throw() {
  return (_last_notification);
}

/**
 *  Set the last notification value.
 *
 *  @param[in] val  The new last notification value.
 */
void escalation::set_last_notification(unsigned int val) throw() {
  _last_notification = val;
}

/**
 *  Get the notification interval.
 *
 *  @return  The notification interval.
 */
unsigned int escalation::get_notification_interval() const throw() {
  return (_notification_interval);
}

/**
 *  Set the notification interval.
 *
 *  @param[in] val  The new notification interval.
 */
void escalation::set_notification_interval(unsigned int val) throw() {
  _notification_interval = val;
}

/**
 *  Parse a line containing the host escalation options.
 *
 *  @param[in] line  The lin containing the host escalation options.
 */
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

/**
 *  Parse a line containing the service escalation options.
 *
 *  @param[in] line  The line containing the service escalation options.
 */
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
