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

#include "com/centreon/broker/notification/builders/node_by_node_id_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Construct the object.
 *
 *  @param[in,out] set  The set to fill.
 */
node_by_node_id_builder::node_by_node_id_builder(
                           QHash<objects::node_id, objects::node::ptr>& map)
  : _map(map) {}

/**
 *  Add a node to the builder.
 *
 *  @param[in] node The node.
 */
void node_by_node_id_builder::add_node(node::ptr node) {
  _map.insert(node->get_node_id(), node);
}
