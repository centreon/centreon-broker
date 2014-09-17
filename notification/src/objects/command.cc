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
#include "com/centreon/broker/notification/objects/command.hh"

using namespace com::centreon::broker::notification;

command::command(std::string const& base_command) :
  _base_command(base_command) {}

command::command(command const& obj) {
  command::operator=(obj);
}

command& command::operator=(command const& obj) {
  if (this != &obj) {
    _name = obj._name;
    _base_command = obj._base_command;
  }
  return (*this);
}

std::string const& command::get_name() const throw() {
  return (_name);
}

void command::set_name(std::string const& name) {
  _name = name;
}

std::string command::resolve(neb::host_status) {
  std::string ret(_base_command);

  //string::replace_all(ret, "$HOSTADDRESS$", );
  return (ret);
}

std::string command::resolve(neb::service_status) {
  return (_base_command); // STUB
}
