/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include "com/centreon/broker/rrd/lib.hh"

#include <gtest/gtest.h>

#include <fstream>

#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;

TEST(RRDLib, Simple) {
  rrd::lib lib{"/tmp/", 42};

  ::remove("/tmp/rrd_test_file");
  ASSERT_THROW(lib.open("/tmp/rrd_test_file"), msg_fmt);
  std::ofstream ofs;
  ofs.open("/tmp/rrd_test_file");
  ofs.close();
  ASSERT_NO_THROW(lib.open("/tmp/rrd_test_file", 3600, 200, 1, 60));
  lib.begin();
  lib.update(200, "3.2989");
  lib.update(-1, "3.2989");
  lib.commit();
  lib.close();
  lib.remove("/tmp/dsajadsllkhdalk");
  lib.remove("/tmp/rrd_test_file");
}
