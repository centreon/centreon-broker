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

#include "com/centreon/broker/notification/objects/escalation.hh"

using namespace com::centreon::broker::notification;

escalation::escalation() :
  _type(unknown),
  _escalation_options(0),
  _first_notification(0),
  _last_notification(0),
  _notification_interval(0) {

}

escalation::escalation(escalation const& obj) :
  _type(obj._type),
  _contactgroups(obj._contactgroups),
  _contacts(obj._contacts),
  _escalation_options(obj._escalation_options),
  _escalation_period(obj._escalation_period),
  _first_notification(obj._first_notification),
  _hostgroups(obj._hostgroups),
  _hosts(obj._hosts),
  _last_notification(obj._last_notification),
  _notification_interval(obj._notification_interval),
  _servicegroups(obj._servicegroups),
  _service_description(obj._service_description) {

}

escalation& escalation::operator=(escalation const& obj) {
  if (this != &obj) {
    _type = obj._type;
    _contactgroups = obj._contactgroups;
    _contacts = obj._contacts;
    _escalation_options = obj._escalation_options;
    _escalation_period = obj._escalation_period;
    _first_notification = obj._first_notification;
    _hostgroups = obj._hostgroups;
    _hosts = obj._hosts;
    _last_notification = obj._last_notification;
    _notification_interval = obj._notification_interval;
    _servicegroups = obj._servicegroups;
    _service_description = obj._service_description;
  }
  return (*this);
}

bool escalation::is_host_escalation() const throw() {
  return (_type == host);
}

bool escalation::is_service_escalation() const throw() {
  return (_type == service);
}

void escalation::set_is_host_escalation(bool val) throw() {
  if (val)
    _type = host;
  else
    _type = service;
}

void escalation::set_is_service_escalation(bool val) throw() {
  if (val)
    _type = service;
  else
    _type = host;
}

group const& escalation::get_contactgroups() const throw() {
  return (_contactgroups);
}

void escalation::set_contactgroups(group const& val) {
  _contactgroups = val;
}

group const& escalation::get_contacts() const throw() {
  return (_contacts);
}

void escalation::set_contacts(group const& val) {
  _contacts = val;
}

unsigned short escalation::get_escalation_options() const throw() {
  return (_escalation_options);
}

void escalation::set_escalation_options(unsigned int val) throw() {
  _escalation_options = val;
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

group const& escalation::get_hostgroups() const throw() {
  return (_hostgroups);
}

void escalation::set_hostgroups(group const& val) {
  _hostgroups = val;
}

group const& escalation::get_hosts() const throw() {
  return (_hosts);
}

void escalation::set_hosts(group const& val) {
  _hosts = val;
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

group const& escalation::get_servicegroups() const throw() {
  return (_servicegroups);
}

void escalation::set_servicegroups(group const& val) {
  _servicegroups = val;
}
