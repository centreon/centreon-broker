/*
** Copyright 2011 Merethis
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

#include <QFile>
#include <QString>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/logging/temp_logger.hh"
#include "test/logging/file/common.hh"

using namespace com::centreon::broker;

/**
 *  Check that the temp_logger class works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  logging::manager::load();

  // Return value.
  int retval(0);

  // Log file path.
  QString file_path(temp_file_path());
  QFile::remove(file_path);

  try {
    {
      // Open log file object.
      logging::file f(file_path);
      f.with_thread_id(false);
      f.with_timestamp(false);

      // Add logging object.
      logging::manager::instance().log_on(
        f,
        logging::config_type
        | logging::debug_type
        | logging::error_type
        | logging::info_type,
        logging::low);

      // Create and destroy temp_logger.
      {
        logging::temp_logger tl(
          logging::config_type,
          logging::high,
          true);
      }
    }

    // At this point, an empty line should have been logged.
    logging::manager::unload();

    // Check file content.
    retval |= !check_content(
      file_path,
      "^config:  $",
      1);
  }
  catch (...) {
    retval = 1;
  }

  // Remove file.
  QFile::remove(file_path);

  // Return check result.
  return (retval);
}
