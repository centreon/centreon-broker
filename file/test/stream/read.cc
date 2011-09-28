/*
** Copyright 2011 Merethis
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

#include <QDir>
#include <QFile>
#include <string.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "centreon_broker_unit_test"

/**
 *  Check that file stream can be properly written to.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Generate file name.
  QString filename(QDir::tempPath());
  filename.append("/" TEMP_FILE_NAME);

  // Remove old file.
  QFile::remove(filename);

  // Return value.
  int retval(0);

  // Write data to file.
  char const buffer[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  {
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly))
      retval |= 1;
    for (unsigned int i = 0; i < 10000; ++i) {
      unsigned int current(0);
      while (!retval && (current < sizeof(buffer) - 1)) {
        qint64 wb(f.write(buffer + current,
                  sizeof(buffer) - 1 - current));
        if (wb <= 0)
          retval = 1;
        else {
          current += wb;
          f.waitForBytesWritten(-1);
        }
      }
    }
    f.close();
  }

  // Open file stream.
  file::stream fs(filename, QIODevice::ReadOnly);

  // Read data.
  unsigned int bufferc(0);
  QSharedPointer<io::raw> raw;
  unsigned int rawc(0);
  for (unsigned int count = 0; !retval && (count < 10000);) {
    // Read data.
    if (raw.isNull()) {
      QSharedPointer<io::data> d(fs.read());
      if (d.isNull() || ("com::centreon::broker::io::raw" != d->type()))
        retval |= 1;
      else {
        raw = d.staticCast<io::raw>();
        rawc = 0;
      }
    }

    if (!retval) {
      // Compare data.
      unsigned int cb(((raw->size() - rawc)
                         < (sizeof(buffer) - 1 - bufferc))
                      ? (raw->size() - rawc)
                      : (sizeof(buffer) - 1 - bufferc));
      retval |= memcmp(raw->QByteArray::data() + rawc,
                       buffer + bufferc,
                       cb);

      // Adjust buffers.
      bufferc += cb;
      if (bufferc == (sizeof(buffer) - 1)) {
        ++count;
        bufferc = 0;
      }
      rawc += cb;
      if (rawc == raw->size())
        raw.clear();
    }
  }
  // EOF must be reached.
  retval |= !fs.read().isNull();

  // Remove temporary file.
  QFile::remove(filename);

  return (retval);
}
