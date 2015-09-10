/*
** Copyright 2011-2012 Centreon
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

#define MAX_SIZE 2000000ull

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
  QString backup_file_path(file_path);
  backup_file_path.append(".old");
  QFile::remove(file_path);
  QFile::remove(backup_file_path);

  try {
    {
      // Open log file object.
      logging::file f(file_path, MAX_SIZE);
      f.with_thread_id(false);
      f.with_timestamp(false);

      // Write log messages.
      for (unsigned int i = 0; i < 200000; ++i)
        write_log_messages(&f);
    }

    // Check files sizes.
    QFile f1(file_path);
    QFile f2(backup_file_path);
    qint64 s1(f1.size());
    qint64 s2(f2.size());
    retval = !(f1.exists()
               && f2.exists()
               && (s1 >= 0)
               && (s1 <= static_cast<qint64>(MAX_SIZE))
               && (s2 >= 0)
               && (s2 <= static_cast<qint64>(MAX_SIZE)));
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    retval = 1;
  }
  catch (...) {
    retval = 1;
  }

  // Remove files.
  QFile::remove(file_path);
  QFile::remove(backup_file_path);

  // Return check result.
  return (retval);
}
