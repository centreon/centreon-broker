/*
** Copyright 2011 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
  s1.is_for_rebuild = true;
  s1.rrd_len = 180;
  s1.state = 1;

  // Second object.
  storage::status s2(s1);

  // Change first object.
  s1.ctime = 741258;
  s1.index_id = 3612;
  s1.interval = 36;
  s1.is_for_rebuild = false;
  s1.rrd_len = 900;
  s1.state = 2;

  // Check objects properties values.
  return ((s1.ctime != 741258)
          || (s1.index_id != 3612)
          || (s1.interval != 36)
          || (s1.is_for_rebuild != false)
          || (s1.rrd_len != 900)
          || (s1.state != 2)
          || (s2.ctime != 123456789)
          || (s2.index_id != 42)
          || (s2.interval != 24)
          || (s2.is_for_rebuild != true)
          || (s2.rrd_len != 180)
          || (s2.state != 1));
}
