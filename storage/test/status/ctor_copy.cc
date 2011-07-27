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

#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;

/**
 *  Check that the status copy constructor works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  storage::status s1;
  s1.ctime = 123456789;
  s1.index_id = 42;
  s1.interval = 24;
  s1.rrd_len = 180;
  s1.state = 1;

  // Second object.
  storage::status s2(s1);

  // Change first object.
  s1.ctime = 741258;
  s1.index_id = 3612;
  s1.interval = 36;
  s1.rrd_len = 900;
  s1.state = 2;

  // Check objects properties values.
  return ((s1.ctime != 741258)
          || (s1.index_id != 3612)
          || (s1.interval != 36)
          || (s1.rrd_len != 900)
          || (s1.state != 2)
          || (s2.ctime != 123456789)
          || (s2.index_id != 42)
          || (s2.interval != 24)
          || (s2.rrd_len != 180)
          || (s2.state != 1));
}
