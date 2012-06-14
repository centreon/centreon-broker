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

#include <iostream>
#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <string.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "centreon_broker_unit_test"

/**
 *  Read some data.
 *
 *  @param[in] count Number of lines to write.
 */
static int read_some(file::stream& f, unsigned int count) {
  // Static data.
  static unsigned int already_read(0);
  static QByteArray buffer;

  // Find all numbers.
  int retval(0);
  for (unsigned int i(0); i < count; ++i) {
    // Find carriage return.
    int index(buffer.indexOf('\n'));
    while (-1 == index) {
      misc::shared_ptr<io::data> d(f.read());
      if (d->type() != "com::centreon::broker::io::raw")
        return (1);
      misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
      buffer.append(*r);
      index = buffer.indexOf('\n');
    }

    // Number.
    QByteArray nb(buffer.left(index));

    // Skip data.
    buffer.remove(0, index + 1);

    // Check content.
    if (nb.toUInt() != ++already_read) {
      std::cerr << "expected: " << already_read
        << " got: " << nb.toUInt()
        << " string: '" << nb.data() << "'" << std::endl;
      retval |= 1;
    }
  }
  return (retval);
}

/**
 *  Write some data.
 *
 *  @param[in] count Number of lines to write.
 */
static int write_some(file::stream& f, unsigned int count) {
  static unsigned int written(0);
  for (unsigned int i = 0; i < count; ++i) {
    char buffer[32];
    ++written;
    snprintf(buffer, sizeof(buffer) - 1, "%u\n", written);
    misc::shared_ptr<io::raw> data(new io::raw);
    data->append(buffer);
    f.write(data.staticCast<io::data>());
  }
  return (0);
}

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

  // Open file stream.
  file::stream fs(filename, QIODevice::ReadWrite);

  // Return value.
  int retval(0);

  // Alternate read and writes.
  retval |= write_some(fs, 300);
  retval |= read_some(fs, 100);
  retval |= write_some(fs, 40);
  retval |= read_some(fs, 200);
  retval |= write_some(fs, 60);
  retval |= read_some(fs, 50);
  retval |= write_some(fs, 200);
  retval |= read_some(fs, 225);
  retval |= read_some(fs, 25);

  // No more data is available.
  try {
    fs.read();
    retval |= 1;
  }
  catch (io::exceptions::shutdown const& s) {
    (void)s;
  }

  // Remove temporary file.
  QFile::remove(filename);

  return (retval);
}
