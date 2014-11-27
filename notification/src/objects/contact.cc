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

#include "com/centreon/broker/notification/objects/contact.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
contact::contact() :
  _id(0),
  _can_submit_commands(false),
  _host_notifications_enabled(false),
  _host_notification_options(host_none),
  _retain_nonstatus_information(false),
  _retain_status_information(false),
  _service_notification_options(service_none),
  _service_notifications_enabled(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
contact::contact(contact const& obj) {
  contact::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
contact& contact::operator=(contact const& obj) {
  if (this != &obj) {
    _id = obj._id;
    _address =  obj._address;
    _alias = obj._alias;
    _can_submit_commands = obj._can_submit_commands;
    _contact_name = obj._contact_name;
    _customvariables = obj._customvariables;
    _email = obj._email;
    _host_notifications_enabled = obj._host_notifications_enabled;
    _host_notification_options = obj._host_notification_options;
    _host_notification_period = obj._host_notification_period;
    _retain_nonstatus_information = obj._retain_nonstatus_information;
    _retain_status_information = obj._retain_status_information;
    _pager = obj._pager;
    _service_notification_options = obj._service_notification_options;
    _service_notification_period = obj._service_notification_period;
    _service_notifications_enabled = obj._service_notifications_enabled;
  }
  return *this;
}

/**
 *  Get the id of the contact.
 *
 *  @return  The id of the contact.
 */
unsigned int contact::get_id() const throw() {
  return (_id);
}

/**
 *  Set the id of the contact.
 *
 *  @param[in] val  The id of the contact.
 */
void contact::set_id(unsigned int val) {
  _id = val;
}

/**
 *  Get the addresses of the contact.
 *
 *  @return  The addresses of the contact.
 */
std::vector<std::string> const& contact::get_address() const throw() {
  return (_address);
}

/**
 *  Add an address to the contact.
 *
 *  @param[in] value  The address to add.
 */
void contact::add_address(std::string const& value) {
  _address.push_back(value);
}

/**
 *  Get the alias of the contact.
 *
 *  @return  The alias of the contact.
 */
std::string const& contact::get_alias() const throw() {
  return (_alias);
}

/**
 *  Set the alias of the contact.
 *
 *  @param[in] value  The new alias of the contact.
 */
void contact::set_alias(std::string const& value) {
  _alias = value;
}

/**
 *  Can this contact submit commands?
 *
 *  @return  True if this contact can submit commands.
 */
bool contact::get_can_submit_commands() const throw() {
  return (_can_submit_commands);
}

/**
 *  Set if this contact can submit commands.
 *
 *  @param[in] value  The new value of the submit command flag.
 */
void contact::set_can_submit_commands(bool value) throw() {
  _can_submit_commands = value;
}

/**
 *  Get the name of the contact.
 *
 *  @return  The name of the contact.
 */
std::string const& contact::get_name() const throw() {
  return (_contact_name);
}

/**
 *  Set the name of the contact.
 *
 *  @param[in] value  The new name of the contact.
 */
void contact::set_name(std::string const& value) {
  _contact_name = value;
}

/**
 *  Get the custom variables of this contact.
 *
 *  @return  The custom variables of this contact.
 */
std::map<std::string, std::string> const&
                         contact::get_customvariables() const throw() {
  return (_customvariables);
}

/**
 *  Add a custom variable to this contact.
 *
 * @param[in] name  The name of the new custom variable.
 * @param[in] value The value of the new custom variable.
 */
void contact::add_customvariables(
                std::string const& name,
                std::string const& value) {
  _customvariables[name] = value;
}

/**
 *  Get the email of the contact.
 *
 *  @return  The email of the contact.
 */
std::string const& contact::get_email() const throw() {
  return (_email);
}

/**
 *  Set the email of the contact.
 *
 *  @param[in] value  The new email of the contact.
 */
void contact::set_email(std::string const& value) {
  _email = value;
}

/**
 *  Are the host notifications enabled for this contact?
 *
 *  @return  True if the host notifications are enabled for this contact.
 */
bool contact::get_host_notifications_enabled() const throw() {
  return (_host_notifications_enabled);
}

/**
 *  Set if the host notifications are enabled for this contact.
 *
 *  @param[in] value  Set the new value for the host notifications are enabled flag.
 */
void contact::set_host_notifications_enabled(bool value) throw() {
  _host_notifications_enabled = value;
}

contact::host_action_on contact::get_host_notification_options() const throw() {
  return (_host_notification_options);
}

void contact::set_host_notification_options(host_action_on value) throw() {
  _host_notification_options = value;
}

void contact::set_host_notification_option(host_action_on value) throw() {
  _host_notification_options
    = (host_action_on)(_host_notification_options | value);
}

bool contact::can_be_notified_for_host(host_action_on value) const throw() {
  return (_host_notification_options & value);
}

std::string const& contact::get_host_notification_period() const throw() {
  return (_host_notification_period);
}

void contact::set_host_notification_period(std::string const& value) {
  _host_notification_period = value;
}

bool contact::get_retain_nonstatus_information() const throw() {
  return (_retain_nonstatus_information);
}

void contact::set_retain_nonstatus_information(bool value) throw() {
  _retain_nonstatus_information = value;
}

bool contact::get_retain_status_information() const throw() {
  return (_retain_status_information);
}

void contact::set_retain_status_information(bool value) throw() {
  _retain_status_information = value;
}

/**
 *  Get the pager of the contact.
 *
 *  @return  The pager of the contact.
 */
std::string const& contact::get_pager() const throw() {
  return (_pager);
}

/**
 *  Set the pager of the contact.
 *
 *  @param[in] value  The new value for the pager of the contact.
 */
void contact::set_pager(std::string const& value) {
  _pager = value;
}

contact::service_action_on contact::get_service_notification_options() const throw() {
  return (_service_notification_options);
}

void contact::set_service_notification_options(service_action_on value) throw() {
  _service_notification_options = value;
}

void contact::set_service_notification_option(
                service_action_on value) throw() {
  _service_notification_options =
    (service_action_on)(_service_notification_options | value);
}

bool contact::can_be_notified_for_service(
                service_action_on value) const throw() {
  return (_service_notification_options & value);
}

std::string const& contact::get_service_notification_period() const throw() {
  return (_service_notification_period);
}

void contact::set_service_notification_period(std::string const& value) {
  _service_notification_period = value;
}

bool contact::get_service_notifications_enabled() const throw() {
  return (_service_notifications_enabled);
}

void contact::set_service_notifications_enabled(bool value) throw() {
  _service_notifications_enabled = value;
}

bool contact::can_be_notified(node_state state, bool isHost) const throw() {
  if (isHost) {
    if (state == node_state::ok
        && can_be_notified_for_host(host_up))
      return (true);
    else if (state == node_state::host_down
             && can_be_notified_for_host(host_down))
      return (true);
    else if (state == node_state::host_unreachable
             && can_be_notified_for_host(host_unreachable))
      return (true);
  }
  else {
    if (state == node_state::ok
        && can_be_notified_for_service(service_ok))
      return (true);
    else if (state == node_state::service_warning
             && can_be_notified_for_service(service_warning))
      return (true);
    else if (state == node_state::service_critical
             && can_be_notified_for_service(service_critical))
      return (true);
    else if (state == node_state::service_unknown
             && can_be_notified_for_service(service_unknown))
      return (true);
  }
  return (false);
}
