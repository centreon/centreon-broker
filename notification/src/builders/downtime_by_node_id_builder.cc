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
    QMultiHash<node_id, downtime::ptr>& table) :
  _table(table) {}

void downtime_by_node_id_builder::add_downtime(unsigned int downtime_id,
                                               downtime::ptr downtime) {
  _table.insert(node_id(downtime->get_host_id(), downtime->get_service_id()),
                downtime);
}
