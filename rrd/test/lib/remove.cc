/*
** Copyright 2011-2013 Centreon
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

#include <cstdlib>
#include <ctime>
#include <QDir>
#include <QString>
#include <unistd.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/rrd/lib.hh"

using namespace com::centreon::broker;

#define TEMP_FILE "broker_rrd_lib_remove"

/**
 *  Check RRD file remove.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Temporary file path.
  std::string dir_path(QDir::tempPath().toStdString());
  std::string file_path(dir_path);
  file_path.append("/" TEMP_FILE);
  ::remove(file_path.c_str());

  // RRD library object.
  rrd::lib lib(dir_path, 16);
  lib.open(
    file_path,
    90 * 24 * 60 * 60,
    time(NULL) - 7 * 24 * 60 * 60,
    60);

  lib.remove(file_path);

  // Check file exists.
  bool file_exists(!access(file_path.c_str(), F_OK));

  // Remove temporary file.
  ::remove(file_path.c_str());

  // Cleanup.
  config::applier::deinit();

  return (!file_exists ? EXIT_SUCCESS : EXIT_FAILURE);
}
