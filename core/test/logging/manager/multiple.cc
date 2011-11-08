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

#include <QCoreApplication>
#include <QFile>
#include <QString>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "test/logging/file/common.hh"

using namespace com::centreon::broker;

/**
 *  Check that the logging manager works as expected.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main (int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  logging::manager::load();

  // Return value.
  int retval(0);

  // Log file path.
  QString file_path1(temp_file_path("1"));
  QString file_path2(temp_file_path("2"));
  QFile::remove(file_path1);
  QFile::remove(file_path2);

  try {
    {
      // Open log file objects.
      logging::file f1(file_path1);
      f1.with_thread_id(false);
      f1.with_timestamp(false);
      logging::file f2(file_path2);
      f2.with_thread_id(false);
      f2.with_timestamp(false);

      // Add logging files.
      logging::manager::instance().log_on(
        f1,
        logging::config_type
        | logging::debug_type
        | logging::error_type
        | logging::info_type,
        logging::low);
      logging::manager::instance().log_on(
        f2,
        logging::config_type
        | logging::debug_type
        | logging::error_type
        | logging::info_type,
        logging::low);

      // Write log messages.
      write_log_messages(&logging::manager::instance(), 4);
    }

    // Unload manager.
    logging::manager::unload();

    // Check files content.
    retval |= !check_content(
      file_path1,
      "^[a-z]+: +<MSG>$");
    retval |= !check_content(
      file_path2,
      "^[a-z]+: +<MSG>$");
  }
  catch (...) {
    retval = 1;
  }

  // Remove file.
  QFile::remove(file_path1);
  QFile::remove(file_path2);

  // Return check result.
  return (retval);
}
