/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that the logger configuration class can be copied properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  config::logger l1;
  l1.config(false);
  l1.debug(true);
  l1.error(true);
  l1.info(false);
  l1.level(logging::medium);
  l1.max_size(1456354874ull);
  l1.name("FooBar");
  l1.type(config::logger::file);

  // Second object.
  config::logger l2(l1);

  // Change first object.
  l1.config(true);
  l1.debug(false);
  l1.error(false);
  l1.info(true);
  l1.level(logging::low);
  l1.max_size(484587188ull);
  l1.name("BazQux");
  l1.type(config::logger::standard);

  // Check.
  return ((!l1.config())
          || (l1.debug())
          || (l1.error())
          || (!l1.info())
          || (l1.level() != logging::low)
          || (l1.max_size() != 484587188ull)
          || (l1.name() != "BazQux")
          || (l1.type() != config::logger::standard)
          || (l2.config())
          || (!l2.debug())
          || (!l2.error())
          || (l2.info())
          || (l2.level() != logging::medium)
          || (l2.max_size() != 1456354874ull)
          || (l2.name() != "FooBar")
          || (l2.type() != config::logger::file));
}
