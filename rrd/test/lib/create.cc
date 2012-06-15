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

#include <QDir>
#include <QFile>
#include <QString>
#include <time.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/rrd/lib.hh"

using namespace com::centreon::broker;

#define TEMP_FILE "broker_rrd_lib_create"

/**
 *  Check RRD file creation.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Temporary file path.
  QString file_path(QDir::tempPath());
  file_path.append("/" TEMP_FILE);
  QFile::remove(file_path);

  // RRD library object.
  rrd::lib lib;
  lib.open(
    file_path,
    "C:\\ Used Space",
    90 * 24 * 60 * 60,
    time(NULL) - 7 * 24 * 60 * 60,
    60);

  // Check file exists.
  int retval(!QFile::exists(file_path));

  // Remove temporary file.
  QFile::remove(file_path);

  return (retval);
}
