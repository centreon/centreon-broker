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
#include <cstring>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_file_stream_read.tmp"

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

  // Return value.
  int retval(0);

  // Write data to file.
  char const buffer[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  {
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly))
      retval |= 1;
    {
      char header[8];
      memset(header, 0, sizeof(header));
      header[7] = 8;
      unsigned int size(0);
      while (size != sizeof(header)) {
        size += f.write(header + size, sizeof(header) - size);
        f.waitForBytesWritten(-1);
      }
    }
    for (unsigned int i(0); i < 10000; ++i) {
      unsigned int current(0);
      while (!retval && (current < sizeof(buffer) - 1)) {
        qint64 wb(f.write(
                      buffer + current,
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
  file::stream fs(filename.toStdString());

  // Read data.
  unsigned int bufferc(0);
  std::shared_ptr<io::raw> raw;
  unsigned int rawc(0);
  for (unsigned int count = 0; !retval && (count < 10000);) {
    // Read data.
    if (raw.isNull()) {
      std::shared_ptr<io::data> d;
      fs.read(d, (time_t)-1);
      if (d.isNull() || (io::raw::static_type() != d->type()))
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
      if (static_cast<int>(rawc) == raw->size())
        raw.clear();
    }
  }
  // EOF must be reached.
  try {
    std::shared_ptr<io::data> d;
    fs.read(d, (time_t)-1);
    retval |= 1;
  }
  catch (io::exceptions::shutdown const& s) {
    (void)s;
  }

  // Remove temporary file.
  QFile::remove(filename);

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
