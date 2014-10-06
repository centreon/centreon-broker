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

#include "com/centreon/broker/notification/builders/composed_node_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_node_builder::composed_node_builder() {}

void composed_node_builder::add_node(node::ptr node) {
  for (composed_builder<node_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->add_node(node);
}

void composed_node_builder::connect_node_servicegroup(
        objects::node_id id,
        unsigned int group_id) {
  for (composed_builder<node_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_node_servicegroup(id, group_id);
}

void composed_node_builder::connect_node_hostgroup(
        objects::node_id id,
        unsigned int group_id) {
  for (composed_builder<node_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_node_hostgroup(id, group_id);
}

void composed_node_builder::connect_hostgroup_servicegroup(
        unsigned int hostgroup_id,
        unsigned int servicegroup_id) {
  for (composed_builder<node_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_hostgroup_servicegroup(hostgroup_id, servicegroup_id);
}

void composed_node_builder::connect_hostgroup_parent_hostgroup(
        unsigned int hostgroup_id,
        unsigned int parent_hostgroup_id) {
  for (composed_builder<node_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->connect_hostgroup_parent_hostgroup(hostgroup_id,
                                              parent_hostgroup_id);
}
