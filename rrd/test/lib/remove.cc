/*
** Copyright 2011-2013 Merethis
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
