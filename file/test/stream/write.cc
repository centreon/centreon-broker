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

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_file_stream_write"

/**
 *  Check that file stream can be properly written to.
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
  QString filename(QDir::tempPath());
  filename.append("/" TEMP_FILE_NAME);

  // Remove old file.
  QFile::remove(filename);

  // Generate data packet.
  misc::shared_ptr<io::raw> data(new io::raw);
  data->append("0123456789abcdefghijklmnopqrstuvwxyz");

  {
    // Open file stream for writing.
    file::stream fs(filename, QIODevice::WriteOnly);

    // Write data in file.
    for (unsigned int i = 0; i < 10000; ++i)
      fs.write(data.staticCast<io::data>());
  }

  // Return value.
  int retval(0);

  // Open file to check.
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly))
    retval |= 1;

  // Read and compare data.
  char buffer[36];
  unsigned int count(0);
  unsigned int current(0);
  while (!retval && (count < 10000)) {
    f.waitForReadyRead(-1);
    qint64 rb(f.read(buffer + current, sizeof(buffer) - current));
    if (rb <= 0)
      retval |= 1;
    else {
      current += rb;
      if (current == sizeof(buffer)) {
        retval |= memcmp(buffer, "0123456789abcdefghijklmnopqrstuvwxyz", sizeof(buffer));
        ++count;
        current = 0;
      }
    }
  }
  // EOF must be reached.
  f.waitForReadyRead(-1);
  retval |= (f.read(buffer, 1) > 0);

  // Remove temporary file.
  QFile::remove(filename);

  return (retval);
}
