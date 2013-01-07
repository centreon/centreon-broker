/*
** Copyright 2012-2013 Merethis
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
#include <limits>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
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
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  // Generate file name.
  QString filename[4];
  for (unsigned int i(0); i < 4; ++i)
    filename[i] = QDir::tempPath();
  filename[0].append("/" TEMP_FILE_NAME);
  filename[1].append("/" TEMP_FILE_NAME "1");
  filename[2].append("/" TEMP_FILE_NAME "2");
  filename[3].append("/" TEMP_FILE_NAME "3");

  // Remove old file.
  for (unsigned int i(0); i < 4; ++i)
    ::remove(qPrintable(filename[i]));

  // Generate data packet.
  misc::shared_ptr<io::raw> data(new io::raw);
  data->append("0123456789abcdefghijklmnopqrstuvwxyz");

  {
    // Open file stream for writing.
    file::stream fs(filename[0].toStdString(), 100000);

    // Write data in files.
    for (unsigned int i(0); i < 10000; ++i)
      fs.write(data.staticCast<io::data>());
  }

  // Return value.
  int retval(0);

  // Check files.
  retval = ((QFile(filename[0]).size() != 100000)
            || (QFile(filename[1]).size() != 100000)
            || (QFile(filename[2]).size() != 100000)
            || (QFile(filename[3]).size() != (60000 + 4 * 8)));

  // Remove temporary file.
  for (unsigned int i(0); i < 4; ++i)
    ::remove(qPrintable(filename[i]));

  // Check max file size #1.
  {
    file::stream fs(filename[0].toStdString(), 0);
    retval |= (fs.get_max_size()
               != static_cast<unsigned long long>(
                    std::numeric_limits<long>::max()));
  }

  // Check max file size #2.
  {
    file::stream fs(filename[0].toStdString(), 5);
    retval |= (fs.get_max_size()
               != static_cast<unsigned long long>(
                    std::numeric_limits<long>::max()));
  }

  // Check max fiel size #3.
  {
    file::stream
      fs(
        filename[0].toStdString(),
        std::numeric_limits<unsigned long long>::max() - 123456);
    retval |= (fs.get_max_size()
               != static_cast<unsigned long long>(
                    std::numeric_limits<long>::max()));
  }

  return (retval ? 1 : 0);
}
