/*
** Copyright 2011-2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
