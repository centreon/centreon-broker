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

#include <cmath>
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the perfdata assignment operator works properly.
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
  p1.value_type(storage::perfdata::counter);
  p1.warning(4548.0);

  // Second object.
  storage::perfdata p2;
  p2.critical(2345678.9672374);
  p2.max(834857.9023);
  p2.min(348.239479);
  p2.name("merethis");
  p2.unit("centreon");
  p2.value(8374598345.234);
  p2.value_type(storage::perfdata::absolute);
  p2.warning(0.823745784);

  // Assignment.
  p2 = p1;

  // Change first object.
  p1.critical(9432.5);
  p1.max(123.0);
  p1.min(843.876);
  p1.name("baz");
  p1.unit("qux");
  p1.value(3485.9);
  p1.value_type(storage::perfdata::derive);
  p1.warning(3612.0);

  // Check objects properties values.
  return ((fabs(p1.critical() - 9432.5) > 0.00001)
          || (fabs(p1.max() - 123.0) > 0.00001)
          || (fabs(p1.min() - 843.876) > 0.00001)
          || (p1.name() != "baz")
          || (p1.unit() != "qux")
          || (fabs(p1.value() - 3485.9) > 0.00001)
          || (p1.value_type() != storage::perfdata::derive)
          || (fabs(p1.warning() - 3612.0) > 0.00001)
          || (fabs(p2.critical() - 42.0) > 0.00001)
          || (fabs(p2.max() - 76.3) > 0.00001)
          || (fabs(p2.min() - 567.2) > 0.00001)
          || (p2.name() != "foo")
          || (p2.unit() != "bar")
          || (fabs(p2.value() - 52189.912) > 0.00001)
          || (p2.value_type() != storage::perfdata::counter)
          || (fabs(p2.warning() - 4548.0) > 0.00001));
}
