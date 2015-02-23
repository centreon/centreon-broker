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

#include "com/centreon/broker/notification/builders/groups_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Construct the object.
 *
 *  @param[in,out] hostgroup_map                    A map to fill.
 *  @param[in,out] servicegroup_map                 A map to fill.
 *  @param[in,out] hostgroup_to_servicegroups_map   A map to fill.
 *  @param[in,out] hostgroup_parent_relation_map    A map to fill.
 */
groups_builder::groups_builder(
    QMultiHash<objects::node_id, unsigned int>& hostgroup_map,
    QMultiHash<objects::node_id, unsigned int>& servicegroup_map,
    QMultiHash<unsigned int, unsigned int>& hostgroup_to_servicegroups_map,
    QMultiHash<unsigned int, unsigned int>& hostgroup_parent_relation_map) :
  _hostgroup_map(hostgroup_map),
  _servicegroup_map(servicegroup_map),
  _hostgroup_to_servicegroups_map(hostgroup_to_servicegroups_map),
  _hostgroup_parent_relation_map(hostgroup_parent_relation_map) {}

void groups_builder::connect_node_servicegroup(objects::node_id id,
                                               unsigned int group_id) {
  _servicegroup_map.insert(id, group_id);
}

void groups_builder::connect_node_hostgroup(objects::node_id id,
                                            unsigned int group_id) {
  _hostgroup_map.insert(id, group_id);
}

void groups_builder::connect_hostgroup_servicegroup(
              unsigned int hostgroup_id,
              unsigned int servicegroup_id) {
  _hostgroup_to_servicegroups_map.insert(hostgroup_id,
                                         servicegroup_id);
}

void groups_builder::connect_hostgroup_parent_hostgroup(
              unsigned int hostgroup_id,
              unsigned int parent_hostgroup_id) {
  _hostgroup_parent_relation_map.insert(hostgroup_id,
                                        parent_hostgroup_id);
}
