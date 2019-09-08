/*
** Copyright 2012-2013,2015 Centreon
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

#include "test/misc.hh"
#include <unistd.h>
#include <QDir>
#include <QStringList>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

/**
 *  Recursively remove a directory.
 *
 *  @param[in] dir_path Target directory.
 */
void test::recursive_remove(std::string const& dir_path) {
  QStringList entries(QDir(dir_path.c_str()).entryList());
  for (QStringList::const_iterator it(entries.begin()), end(entries.end());
       it != end; ++it) {
    std::ostringstream oss;
    oss << dir_path << "/" << it->toStdString();
    ::remove(oss.str().c_str());
  }
  QDir().rmdir(dir_path.c_str());
}

/**
 *  Sleep for some seconds.
 *
 *  @param[in] units  Number of time units to sleep.
 */
void test::sleep_for(int units) {
  if (units > 0) {
    time_t now(time(NULL));
    time_t target(now + units * MONITORING_ENGINE_INTERVAL_LENGTH);
    while (now < target) {
      sleep(target - now);
      now = time(NULL);
    }
  }
}
