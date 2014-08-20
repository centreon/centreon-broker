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

using namespace com::centreon::broker::notification;

contact::contact() :
  _can_submit_commands(false),
  _host_notifications_enabled(false),
  _host_notification_options(host_none),
  _retain_nonstatus_information(false),
  _retain_status_information(false),
  _service_notification_options(service_none),
  _service_notifications_enabled(false) {}

contact::contact(contact const& obj) :
  _address(obj._address),
  _alias(obj._alias),
  _can_submit_commands(obj._can_submit_commands),
  _contact_name(obj._contact_name),
  _customvariables(obj._customvariables),
  _email(obj._email),
  _host_notifications_enabled(obj._host_notifications_enabled),
  _host_notification_commands(obj._host_notification_commands),
  _host_notification_options(obj._host_notification_options),
  _host_notification_period(obj._host_notification_period),
  _retain_nonstatus_information(obj._retain_nonstatus_information),
  _retain_status_information(obj._retain_status_information),
  _pager(obj._pager),
  _service_notification_commands(obj._service_notification_commands),
  _service_notification_options(obj._service_notification_options),
  _service_notification_period(obj._service_notification_period),
  _service_notifications_enabled(obj._service_notifications_enabled) {}

contact& contact::operator=(contact const& obj) {
  if (this != &obj) {
    _address =  obj._address;
    _alias = obj._alias;
    _can_submit_commands = obj._can_submit_commands;
    _contact_name = obj._contact_name;
    _customvariables = obj._customvariables;
    _email = obj._email;
    _host_notifications_enabled = obj._host_notifications_enabled;
    _host_notification_commands = obj._host_notification_commands;
    _host_notification_options = obj._host_notification_options;
    _host_notification_period = obj._host_notification_period;
    _retain_nonstatus_information = obj._retain_nonstatus_information;
    _retain_status_information = obj._retain_status_information;
    _pager = obj._pager;
    _service_notification_commands = obj._service_notification_commands;
    _service_notification_options = obj._service_notification_options;
    _service_notification_period = obj._service_notification_period;
    _service_notifications_enabled = obj._service_notifications_enabled;
  }
  return *this;
}

std::vector<std::string> const& contact::get_address() const throw() {
  return (_address);
}

void contact::add_address(std::string const& value) {
  _address.push_back(value);
}

std::string const& contact::get_alias() const throw() {
  return (_alias);
}

void contact::set_alias(std::string const& value) {
  _alias = value;
}

bool contact::get_can_submit_commands() const throw() {
  return (_can_submit_commands);
}

void contact::set_can_submit_commands(bool value) throw() {
  _can_submit_commands = value;
}

std::string const& contact::get_name() const throw() {
  return (_contact_name);
}

void contact::set_name(std::string const& value) {
  _contact_name = value;
}

group const& contact::get_group() const throw() {
  return (_contactgroups);
}

void contact::set_group(group const& value) {
  _contactgroups = value;
}

std::map<std::string, std::string> const&
                         contact::get_customvariables() const throw() {
  return (_customvariables);
}

void contact::add_customvariables(std::string const& name,
                                  std::string const& value) {
  _customvariables[name] = value;
}

std::string const& contact::get_email() const throw() {
  return (_email);
}

void contact::set_email(std::string const& value) {
  _email = value;
}

bool contact::get_host_notifications_enabled() const throw() {
  return (_host_notifications_enabled);
}

void contact::set_host_notifications_enabled(bool value) throw() {
  _host_notifications_enabled = value;
}

group const& contact::get_host_notification_commands() const throw() {
  return (_host_notification_commands);
}

void contact::set_host_notification_commands(group const& value) {
  _host_notification_commands = value;
}

contact::host_action_on contact::get_host_notification_options() const throw() {
  return (_host_notification_options);
}

void contact::set_host_notification_options(host_action_on value) throw() {
  _host_notification_options = value;
}

void contact::set_host_notification_option(host_action_on value) throw() {
  _host_notification_options = (host_action_on)(_host_notification_options | value);
}

bool contact::can_be_notified_of_host(host_action_on value) const throw() {
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

std::string const& contact::get_pager() const throw() {
  return (_pager);
}

void contact::set_pager(std::string const& value) {
  _pager = value;
}

group const& contact::get_service_notification_commands() const throw() {
  return (_service_notification_commands);
}

void contact::set_service_notification_commands(group const& value) {
  _service_notification_commands = value;
}

contact::service_action_on contact::get_service_notification_options() const throw() {
  return (_service_notification_options);
}

void contact::set_service_notification_options(service_action_on value) throw() {
  _service_notification_options = value;
}

void contact::set_service_notification_option(service_action_on value) throw() {
  _service_notification_options = (service_action_on)(_service_notification_options | value);
}

bool contact::can_be_notified_of_service(service_action_on value) const throw() {
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
