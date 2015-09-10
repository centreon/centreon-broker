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

#include <exception>
#include <iostream>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "com/centreon/broker/logging/file.hh"
#include "test/logging/file/common.hh"

using namespace com::centreon::broker;

/**
 *  Check that file logging works properly.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Return value.
  int retval(0);

  // Log file path.
  QString file_path(temp_file_path());
  QFile::remove(file_path);

  try {
    {
      // Open log file object.
      logging::file f(file_path);
      f.with_thread_id(true);
      f.with_timestamp(true);

      // Write log messages.
      write_log_messages(&f);
    }

    // Check file content.
    retval |= !check_content(
      file_path,
      "^\\[[0-9]*\\] \\[0x[0-9abcdef]*\\] [a-zA-Z]*: *<MSG>$");
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
