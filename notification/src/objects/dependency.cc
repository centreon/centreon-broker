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

#include "com/centreon/broker/notification/objects/dependency.hh"

using namespace com::centreon::broker::notification;

dependency::dependency() :
  _type(unknown),
  _dependency_type(unknown_dependency),
  _execution_failure_options(0),
  _inherits_parent(false),
  _notification_failure_options(0) {}

dependency::dependency(dependency const& obj) :
  _type(obj._type),
  _dependency_period(obj._dependency_period),
  _dependency_type(obj._dependency_type),
  _dependent_hostgroups(obj._dependent_hostgroups),
  _dependent_hosts(obj._dependent_hosts),
  _dependent_servicegroups(obj._dependent_servicegroups),
  _dependent_service_description(obj._dependent_service_description),
  _execution_failure_options(obj._execution_failure_options),
  _hostgroups(obj._hostgroups),
  _hosts(obj._hosts),
  _inherits_parent(obj._inherits_parent),
  _notification_failure_options(obj._notification_failure_options),
  _servicegroups(obj._servicegroups),
  _service_description(obj._service_description) {}

dependency& dependency::operator=(dependency const& obj) {
  if (this != &obj) {
    _type = obj._type;
    _dependency_period = obj._dependency_period;
    _dependency_type = obj._dependency_type;
    _dependent_hostgroups = obj._dependent_hostgroups;
    _dependent_hosts = obj._dependent_hosts;
    _dependent_servicegroups = obj._dependent_servicegroups;
    _dependent_service_description = obj._dependent_service_description;
    _execution_failure_options = obj._execution_failure_options;
    _hostgroups = obj._hostgroups;
    _hosts = obj._hosts;
    _inherits_parent = obj._inherits_parent;
    _notification_failure_options = obj._notification_failure_options;
    _servicegroups = obj._servicegroups;
    _service_description = obj._service_description;
  }
  return (*this);
}

bool dependency::is_host_dependency() const throw() {
  return (_type == host);
}

void dependency::set_is_host_dependency(bool val) throw() {
  if (val)
    _type = host;
  else
    _type = service;
}

bool dependency::is_service_dependency() const throw() {
  return (_type == service);
}

void dependency::set_is_service_dependency(bool val) throw() {
  if (val)
    _type = service;
  else
    _type = host;
}

std::string const& dependency::get_period() const throw() {
  return (_dependency_period);
}

void dependency::set_period(std::string const& val) {
  _dependency_period = val;
}

dependency::dependency_kind dependency::get_kind() const throw() {
  return (_dependency_type);
}

void dependency::set_kind(dependency::dependency_kind val) throw() {
  _dependency_type = val;
}

group const& dependency::get_dependent_hostgroups() const throw() {
  return (_dependent_hostgroups);
}

void dependency::set_dependent_hostgroups(group const& val) {
  _dependent_hostgroups = val;
}

group const& dependency::get_dependent_hosts() const throw() {
  return (_dependent_hosts);
}

void dependency::set_dependent_hosts(group const& val) {
  _dependent_hosts = val;
}

group const& dependency::get_dependent_servicegroups() const throw() {
  return (_dependent_servicegroups);
}

void dependency::set_dependent_servicegroups(group const& val) {
  _dependent_servicegroups = val;
}

group const& dependency::get_dependent_service_description() const throw() {
  return (_dependent_service_description);
}

void dependency::set_dependent_service_description(group const& val) {
  _dependent_service_description = val;
}

unsigned int dependency::get_execution_failure_options() const throw() {
  return (_execution_failure_options);
}

void dependency::set_execution_failure_options(unsigned int val) throw() {
  _execution_failure_options = val;
}

group const& dependency::get_hostgroups() const throw() {
  return (_hostgroups);
}

void dependency::set_hostgroups(group const& val) {
  _hostgroups = val;
}

group const& dependency::get_hosts() const throw() {
  return (_hosts);
}

void dependency::set_hosts(group const& val) {
  _hosts = val;
}

bool dependency::get_inherits_parent() const throw() {
  return (_inherits_parent);
}

void dependency::set_inherits_parent(bool val) throw() {
  _inherits_parent = val;
}

unsigned int dependency::get_notification_failure_options() const throw() {
  return (_notification_failure_options);
}

void dependency::set_notification_failure_options(unsigned int val) throw() {
  _notification_failure_options = val;
}

group const& dependency::get_servicegroups() const throw() {
  return (_servicegroups);
}

void dependency::set_servicegroups(group const& val) {
  _servicegroups = val;
}

group const& dependency::get_service_description() const throw() {
  return (_service_description);
}

void dependency::set_service_description(group const& val) {
  _service_description = val;
}
