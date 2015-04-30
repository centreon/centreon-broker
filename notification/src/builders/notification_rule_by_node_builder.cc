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

#include "com/centreon/broker/notification/builders/notification_rule_by_node_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Constructor.
 *
 *  @param[in] map  The map to fill.
 */
notification_rule_by_node_builder::notification_rule_by_node_builder(
  QMultiHash<objects::node_id,objects::notification_rule::ptr>& map)
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
