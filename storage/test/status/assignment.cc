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
 *  Check that the status assignment operator works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  storage::status s1;
  s1.ctime = 123456789;
  s1.index_id = 6774;
  s1.interval = 42;
  s1.is_for_rebuild = true;
  s1.rrd_len = 180;
  s1.state = 3;

  // Second object.
  storage::status s2;
  s2.ctime = 654123;
  s2.index_id = 33;
  s2.interval = 78;
  s2.is_for_rebuild = false;
  s2.rrd_len = 666;
  s2.state = 1;

  // Assignment.
  s2 = s1;

  // Change first object.
  s1.ctime = 741258;
  s1.index_id = 12;
  s1.interval = 36;
  s1.is_for_rebuild = false;
  s1.rrd_len = 900;
  s1.state = 2;

  // Check objects properties values.
  return ((s1.ctime != 741258)
          || (s1.index_id != 12)
          || (s1.interval != 36)
          || (s1.is_for_rebuild != false)
          || (s1.rrd_len != 900)
          || (s1.state != 2)
          || (s2.ctime != 123456789)
          || (s2.index_id != 6774)
          || (s2.interval != 42)
          || (s2.is_for_rebuild != true)
          || (s2.rrd_len != 180)
          || (s2.state != 3));
}
