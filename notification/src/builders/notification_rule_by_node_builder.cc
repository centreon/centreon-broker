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

#include "com/centreon/broker/notification/builders/notification_rule_by_node_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Constructor.
 *
 *  @param[in] map  The map to fill.
 */
notification_rule_by_node_builder::notification_rule_by_node_builder(
    QMultiHash<objects::node_id, objects::notification_rule::ptr>& map)
    : _map(map) {}

/**
 *  Add a rule to the builder.
 *
 *  @param[in] id   The id of the rule.
 *  @param[in] con  The rule to add.
 */
void notification_rule_by_node_builder::add_rule(
    unsigned int rule_id,
    objects::notification_rule::ptr rule) {
  (void)rule_id;
  _map.insert(rule->get_node_id(), rule);
}
