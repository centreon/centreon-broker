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

#include <exception>
#include <iostream>
#include <QDir>
#include <QFile>
#include "com/centreon/broker/logging/file.hh"
#include "common.hh"

using namespace com::centreon::broker;

/**
 *  Check that file logging works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(0);

  // Log file path.
  QString file_path(temp_file_path());
  QFile::remove(file_path);

  try {
    {
      // Open log file object.
      logging::file f(file_path);
      f.with_timestamp(true);

      // Write log messages.
      write_log_messages(&f);
    }

    // Check file content.
    retval |= !check_content(file_path, "^\\[[0-9]*\\] [a-zA-Z]*: *<MSG>$");
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    retval = 1;
  }
  catch (...) {
    retval = 1;
  }

  // Remove file.
  QFile::remove(file_path);

  // Return check result.
  return (retval);
}
