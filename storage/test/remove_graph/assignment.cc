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
 *  Check that remove_graph's assignment operator.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  storage::remove_graph r1;
  r1.id = 42;
  r1.is_index = false;

  // Second object.
  storage::remove_graph r2;
  r2.id = 234573485;
  r2.is_index = true;

  // Assignment.
  r2 = r1;

  // Reset base object.
  r1.id = 36;
  r1.is_index = true;

  // Check.
  return ((r1.id != 36)
          || !r1.is_index
          || (r2.id != 42)
          || r2.is_index);
}
