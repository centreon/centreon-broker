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

contact::contact() {}

contact::contact(contact const& obj) :
  _address(obj._address),
  _alias(obj._alias),
  _can_submit_commands(obj._can_submit_commands),
  _contact_name(obj._contact_name),
  _customvariables(obj._custombariables),
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
    _customvariables = obj._custombariables;
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

