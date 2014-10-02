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
#include "com/centreon/broker/notification/builders/command_by_contact_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Construct the object.
 *
 *  @param[in,out] host_table  The table to fill.
 *  @param[in,out] service_table  The table to fill.
 */
command_by_contact_builder::command_by_contact_builder(
    QMultiHash<contact::ptr, std::string>& host_table,
    QMultiHash<contact::ptr, std::string>& service_table) :
  _host_table(host_table),
  _service_table(service_table) {}

void command_by_contact_builder::add_contact(unsigned int id,
                                             contact::ptr con) {
  _cache[id] = con;
}

void command_by_contact_builder::connect_contact_hostcommand(
    unsigned int contact_id,
    std::string const& command_name) {
  if (_cache.contains(contact_id))
  _host_table.insert(_cache[contact_id], command_name);
}

void command_by_contact_builder::connect_contact_servicecommand(
    unsigned int contact_id,
    std::string const& command_name) {
  if (_cache.contains(contact_id))
    _service_table.insert(_cache[contact_id], command_name);
}
