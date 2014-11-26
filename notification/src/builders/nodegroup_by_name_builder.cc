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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/builders/nodegroup_by_name_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

nodegroup_by_name_builder::nodegroup_by_name_builder(
  QHash<std::string, objects::nodegroup::ptr>& map)
  : _map(map) {}

/**
*  Add a nodegroup to the builder.
*
*  @param[in] id   The nodegroup id.
*  @param[in] ndg  The nodegroup.
*/
void nodegroup_by_name_builder::add_nodegroup(
                                  objects::node_id id,
                                  objects::nodegroup::ptr ndg) {
  _map.insert(ndg->get_name(), ndg);
}
