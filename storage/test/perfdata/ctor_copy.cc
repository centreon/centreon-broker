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

#include <math.h>
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the perfdata copy constructor works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  storage::perfdata p1;
  p1.critical(42.0);
  p1.max(76.3);
  p1.min(567.2);
  p1.name("foo");
  p1.unit("bar");
  p1.value(52189.912);
  p1.warning(4548.0);

  // Second object.
  storage::perfdata p2(p1);

  // Change first object.
  p1.critical(9432.5);
  p1.max(123.0);
  p1.min(843.876);
  p1.name("baz");
  p1.unit("qux");
  p1.value(3485.9);
  p1.warning(3612.0);

  // Check objects properties values.
  return ((fabs(p1.critical() - 9432.5) > 0.00001)
          || (fabs(p1.max() - 123.0) > 0.00001)
          || (fabs(p1.min() - 843.876) > 0.00001)
          || (p1.name() != "baz")
          || (p1.unit() != "qux")
          || (fabs(p1.value() - 3485.9) > 0.00001)
          || (fabs(p1.warning() - 3612.0) > 0.00001));
}
