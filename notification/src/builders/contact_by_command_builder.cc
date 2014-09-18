/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/builders/contact_by_command_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

contact_by_command_builder::contact_by_command_builder(
    QMultiHash<std::string, contact::ptr>& table) :
  _table(table) {}

void contact_by_command_builder::add_contact(unsigned int id,
                                             contact::ptr con) {
  _cache[id] = con;
}

void contact_by_command_builder::connect_contact_hostcommand(
    unsigned int contact_id,
    std::string const& command_name) {
  if (_cache.contains(contact_id))
  _table.insert(command_name, _cache[contact_id]);
}

void contact_by_command_builder::connect_contact_servicecommand(
    unsigned int contact_id,
    std::string const& command_name) {
  if (_cache.contains(contact_id))
    _table.insert(command_name, _cache[contact_id]);
}
