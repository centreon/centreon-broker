/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <cstdlib>
#include <limits>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_file_stream_max_size"

/**
 *  Check that file stream can limit its file size.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init(0, "test_broker");

  // Generate file name.
  QString filename[4];
  for (uint32_t i(0); i < 4; ++i)
    filename[i] = QDir::tempPath();
  filename[0].append("/" TEMP_FILE_NAME);
  filename[1].append("/" TEMP_FILE_NAME "1");
  filename[2].append("/" TEMP_FILE_NAME "2");
  filename[3].append("/" TEMP_FILE_NAME "3");

  // Remove old file.
  for (uint32_t i(0); i < 4; ++i)
    ::remove(qPrintable(filename[i]));

  // Generate data packet.
  std::shared_ptr<io::raw> data(new io::raw);
  data->append("0123456789abcdefghijklmnopqrstuvwxyz");

  {
    // Open file stream for writing.
    file::stream fs(filename[0].toStdString(), 100000);

    // Write data in files.
    for (uint32_t i(0); i < 10000; ++i)
      fs.write(data);
  }

  // Return value.
  int retval(0);

  // Check files.
  retval = ((QFile(filename[0]).size() != 100000) ||
            (QFile(filename[1]).size() != 100000) ||
            (QFile(filename[2]).size() != 100000) ||
            (QFile(filename[3]).size() != (60000 + 4 * 8)));

  // Remove temporary file.
  for (uint32_t i(0); i < 4; ++i)
    ::remove(qPrintable(filename[i]));

  // Check max file size #1.
  {
    file::stream fs(filename[0].toStdString(), 0);
    retval |= (fs.get_max_size() != static_cast<unsigned long long>(
                                        std::numeric_limits<long>::max()));
  }

  // Check max file size #2.
  {
    file::stream fs(filename[0].toStdString(), 5);
    retval |= (fs.get_max_size() != static_cast<unsigned long long>(
                                        std::numeric_limits<long>::max()));
  }

  // Check max fiel size #3.
  {
    file::stream fs(filename[0].toStdString(),
                    std::numeric_limits<unsigned long long>::max() - 123456);
    retval |= (fs.get_max_size() != static_cast<unsigned long long>(
                                        std::numeric_limits<long>::max()));
  }

  // Cleanup.
  config::applier::deinit();

  return (retval ? EXIT_FAILURE : EXIT_SUCCESS);
}
