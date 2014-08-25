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

#include "com/centreon/broker/notification/objects/qhash_func.hh"
#include "com/centreon/broker/notification/builders/contact_by_name_builder.hh"

using namespace com::centreon::broker::notification;

contact_by_name_builder::contact_by_name_builder(QHash<std::string, shared_ptr<contact> >& table) :
  _table(table) {}

void contact_by_name_builder::add_contact(unsigned int id,
                                           shared_ptr<contact> con) {
  _table[con->get_name()];
}

void contact_by_name_builder::connect_contact_contactgroup(unsigned int contact_id,
                                                            unsigned int contactgroup_id) {
}

void contact_by_name_builder::connect_contact_hostcommand(unsigned int contact_id,
                                                           unsigned int command_id) {
}

void contact_by_name_builder::connect_contact_servicecommand(unsigned int contact_id,
                                                              unsigned int service_id) {
}

void contact_by_name_builder::add_contact_param(unsigned int contact_id,
                                                 std::string const& key,
                                                 std::string const& value) {
}

void contact_by_name_builder::connect_contact_node_id(unsigned int contact_id,
                                                      node_id id) {
}
