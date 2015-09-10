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
#include "com/centreon/broker/logging/temp_logger.hh"
#include "test/logging/file/common.hh"

using namespace com::centreon::broker;

/**
 *  Check that the temp_logger class works properly.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

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

      // Create and destroy temp_logger objects.
      {
        logging::temp_logger tl1(
          logging::config_type,
          logging::high,
          true);
        logging::temp_logger tl2(tl1);
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
