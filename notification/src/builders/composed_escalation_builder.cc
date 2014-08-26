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

#include "com/centreon/broker/notification/builders/composed_escalation_builder.hh"

using namespace com::centreon::broker::notification;

composed_escalation_builder::composed_escalation_builder() {}

void composed_escalation_builder::add_escalation(unsigned int id,
                                                 shared_ptr<escalation> esc) {
  for (composed_builder<escalation_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->add_escalation(id, esc);
}

void composed_escalation_builder::connect_escalation_node_id(
    unsigned int esc_id,
    node_id id) {
  for (composed_builder<escalation_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_escalation_node_id(esc_id, id);
}

void composed_escalation_builder::connect_escalation_contactgroup(
    unsigned int id,
    unsigned int contactgroup_id) {
  for (composed_builder<escalation_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_escalation_contactgroup(id, contactgroup_id);
}

void composed_escalation_builder::connect_escalation_hostgroup(
    unsigned int id,
    unsigned int hostgroup_id) {
  for (composed_builder<escalation_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_escalation_hostgroup(id, hostgroup_id);
}

void composed_escalation_builder::connect_escalation_servicegroup(
    unsigned int id,
    unsigned int servicegroup_id) {
  for (composed_builder<escalation_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_escalation_servicegroup(id, servicegroup_id);
}
