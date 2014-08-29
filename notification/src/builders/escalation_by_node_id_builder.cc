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

#include "com/centreon/broker/notification/builders/escalation_by_node_id_builder.hh"

using namespace com::centreon::broker::notification;

escalation_by_node_id_builder::escalation_by_node_id_builder(
    QMultiHash<node_id, escalation::ptr>& table) :
  _table(table) {}

void escalation_by_node_id_builder::add_escalation(unsigned int id,
                                                   escalation::ptr esc) {
  _cache[id] = esc;
}

void escalation_by_node_id_builder::connect_escalation_node_id(
    unsigned int esc_id,
    node_id id) {
  if (_cache.contains(esc_id))
    _table.insert(id, _cache[esc_id]);
}
