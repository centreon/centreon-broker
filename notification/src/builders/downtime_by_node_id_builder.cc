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

#include "com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Construct the object.
 *
 *  @param[in,out] table  The table to fill.
 */
downtime_by_node_id_builder::downtime_by_node_id_builder(
                               QMultiHash<node_id, downtime::ptr>& table)
  : _table(table) {}

/**
 *  Add a downtime to the builder.
 *
 *  @param[in] id The id of the downtime.
 *  @param[in] downtime The downtime.
 */
void downtime_by_node_id_builder::add_downtime(
                                    unsigned int downtime_id,
                                    downtime::ptr downtime) {
  _cache[downtime_id] = downtime;
}

/**
 *  Connect a downtime to a node_id.
 *
 *  @param[in] downtime  The id of the downtime.
 *  @param[in] node_id   The node id associated with this downtime.
 */
void downtime_by_node_id_builder::connect_downtime_to_node(
                                    unsigned int downtime_id,
                                    node_id id) {
  if (_cache.contains(downtime_id))
    _table.insert(id, _cache.value(downtime_id));
}
