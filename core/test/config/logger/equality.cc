/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that equality operator works with logger configuration.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  config::logger l1;
  l1.config(true);
  l1.debug(true);
  l1.error(true);
  l1.info(true);
  l1.level(logging::HIGH);
  l1.name("FooBar");
  l1.type(config::logger::standard);

  // Second object.
  config::logger l2(l1);

  // Third object.
  config::logger l3(l1);
  l3.config(false);

  // Check
  return (!(l1 == l2) || (l2 == l3));
}
