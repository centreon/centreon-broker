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

#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"

using namespace com::centreon::broker::notification;

dependency_by_node_id_builder::dependency_by_node_id_builder(
    QMultiHash<node_id, shared_ptr<dependency> >& by_child_id,
    QMultiHash<node_id, shared_ptr<dependency> >& by_parent_id) :
  _table_child_id(by_child_id),
  _table_parent_id(by_parent_id) {}

void dependency_by_node_id_builder::add_dependency(
    unsigned int id,
    shared_ptr<dependency> d) {
  _cache[id] = d;
}

void dependency_by_node_id_builder::dependency_node_id_parent_relation(
    unsigned int dep_id,
    node_id id) {
  _table_parent_id.insert(id, _cache[dep_id]);
}

void dependency_by_node_id_builder::dependency_node_id_child_relation(
    unsigned int dep_id,
    node_id id) {
  _table_child_id.insert(id, _cache[dep_id]);
}
