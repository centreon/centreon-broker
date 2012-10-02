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

#include "com/centreon/broker/storage/rebuild.hh"

using namespace com::centreon::broker;

/**
 *  Check that rebuild's assignment operator.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  storage::rebuild r1;
  r1.end = true;
  r1.metric_id = 42;

  // Second object.
  storage::rebuild r2;
  r2.end = false;
  r2.metric_id = 234573485;

  // Assignment.
  r2 = r1;

  // Reset base object.
  r1.end = false;
  r1.metric_id = 36;

  // Check.
  return (r1.end
          || (r1.metric_id != 36)
          || !r2.end
          || (r2.metric_id != 42));
}
