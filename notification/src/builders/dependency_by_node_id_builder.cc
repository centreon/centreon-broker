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

#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Construct the object.
 *
 *  @param[in,out] by_child_id  The table of dependency by child id to fill.
 *  @param[in,out] by_parent_id  The table of dependency by parent id to fill.
 */
dependency_by_node_id_builder::dependency_by_node_id_builder(
    QMultiHash<node_id, dependency::ptr>& by_child_id,
    QMultiHash<node_id, dependency::ptr>& by_parent_id)
    : _table_child_id(by_child_id), _table_parent_id(by_parent_id) {}

void dependency_by_node_id_builder::add_dependency(uint32_t id,
                                                   dependency::ptr d) {
  _cache[id] = d;
}

void dependency_by_node_id_builder::dependency_node_id_parent_relation(
    uint32_t dep_id,
    node_id id) {
  if (_cache.contains(dep_id))
    _table_parent_id.insert(id, _cache[dep_id]);
}

void dependency_by_node_id_builder::dependency_node_id_child_relation(
    uint32_t dep_id,
    node_id id) {
  if (_cache.contains(dep_id))
    _table_child_id.insert(id, _cache[dep_id]);
}
