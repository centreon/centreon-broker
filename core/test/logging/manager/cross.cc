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
  QString file_path1(temp_file_path());
  QString file_path2(temp_file_path());
  QString file_path3(temp_file_path());
  QFile::remove(file_path1);
  QFile::remove(file_path2);
  QFile::remove(file_path3);

  try {
    {
      // Open log file objects.
      logging::file f1(file_path1);
      f1.with_thread_id(false);
      f1.with_timestamp(false);
      logging::file f2(file_path2);
      f2.with_thread_id(false);
      f2.with_timestamp(false);
      logging::file f3(file_path3);
      f3.with_thread_id(false);
      f3.with_timestamp(false);

      // Add logging files.
      logging::manager::instance().log_on(
        f1,
        logging::config_type
        | logging::error_type,
        logging::high);
      logging::manager::instance().log_on(
        f2,
        logging::config_type
        | logging::debug_type,
        logging::low);
      logging::manager::instance().log_on(
        f3,
        logging::info_type,
        logging::medium);

      // Write log messages.
      write_log_messages(&logging::manager::instance(), 8);
    }

    // Unload manager.
    logging::manager::unload();

    // Check files content.
    char const* const content1[] = { MSG1, NULL };
    retval |= !check_content(
      file_path1,
      "^[a-z]+: +<MSG>$",
      sizeof(content1) / sizeof(*content1) - 1,
      content1);
    char const* const content2[] = { MSG1, MSG2, MSG5, MSG6, NULL };
    retval |= !check_content(
      file_path2,
      "^[a-z]+: +<MSG>$",
      sizeof(content2) / sizeof(*content2) - 1,
      content2);
    char const* const content3[] = { MSG4, MSG7, NULL };
    retval |= !check_content(
      file_path3,
      "^[a-z]+: +<MSG>$",
      sizeof(content3) / sizeof(*content3) - 1,
      content3);
  }
  catch (...) {
    retval = 1;
  }

  // Remove file.
  QFile::remove(file_path1);
  QFile::remove(file_path2);
  QFile::remove(file_path3);

  // Return check result.
  return (retval);
}
