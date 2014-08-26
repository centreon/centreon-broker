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

#include "com/centreon/broker/notification/builders/composed_contact_builder.hh"

using namespace com::centreon::broker::notification;

composed_contact_builder::composed_contact_builder() {}

void composed_contact_builder::add_contact(unsigned int id,
                                           shared_ptr<contact> con) {
  for (composed_builder<contact_builder>::iterator it(begin()),
       end_it(end());
       it != end_it;
       ++it)
    (*it)->add_contact(id, con);
}

void composed_contact_builder::connect_contact_contactgroup(unsigned int contact_id,
                                                            unsigned int contactgroup_id) {
  for (composed_builder<contact_builder>::iterator it(begin()),
       end_it(end());
       it != end_it;
       ++it)
    (*it)->connect_contact_contactgroup(contact_id, contactgroup_id);
}

void composed_contact_builder::connect_contact_hostcommand(unsigned int contact_id,
                                                           unsigned int command_id) {
  for (composed_builder<contact_builder>::iterator it(begin()),
       end_it(end());
       it != end_it;
       ++it)
    (*it)->connect_contact_hostcommand(contact_id, command_id);
}

void composed_contact_builder::connect_contact_servicecommand(unsigned int contact_id,
                                                              unsigned int service_id) {
  for (composed_builder<contact_builder>::iterator it(begin()),
       end_it(end());
       it != end_it;
       ++it)
    (*it)->connect_contact_servicecommand(contact_id, service_id);
}

void composed_contact_builder::add_contact_param(unsigned int contact_id,
                                                 std::string const& key,
                                                 std::string const& value) {
  for (composed_builder<contact_builder>::iterator it(begin()),
       end_it(end());
       it != end_it;
       ++it)
    (*it)->add_contact_param(contact_id, key, value);
}

void composed_contact_builder::connect_contact_node_id(unsigned int contact_id,
                                                       node_id id) {
  for (composed_builder<contact_builder>::iterator it(begin()),
       end_it(end());
       it != end_it;
       ++it)
    (*it)->connect_contact_node_id(contact_id, id);
}
