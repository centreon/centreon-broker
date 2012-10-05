/*
** Copyright 2012 Merethis
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

#include "com/centreon/broker/storage/remove_graph.hh"

using namespace com::centreon::broker;

/**
 *  Check that the remove_graph object properly default constructs.
 *
 *  @return 0 on success.
 */
int main() {
  // Build object.
  storage::remove_graph r;

  // Check properties values.
  return ((r.id != 0) || r.is_index);
}
