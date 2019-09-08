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
#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <cstring>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_file_stream_mixed"

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
      std::shared_ptr<io::data> d;
      f.read(d, (time_t)-1);
      if (d->type() != io::raw::static_type())
        return (1);
      std::shared_ptr<io::raw> r(d.staticCast<io::raw>());
      buffer.append(*r);
      index = buffer.indexOf('\n');
    }

    // Number.
    QByteArray nb(buffer.left(index));

    // Skip data.
    buffer.remove(0, index + 1);

    // Check content.
    if (nb.toUInt() != ++already_read) {
      std::cerr << "expected: " << already_read << " got: " << nb.toUInt()
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
    std::shared_ptr<io::raw> data(new io::raw);
    data->append(buffer);
    f.write(data);
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
  file::stream fs(filename.toStdString(), 200);

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
    std::shared_ptr<io::data> d;
    fs.read(d, (time_t)-1);
    retval |= 1;
  } catch (io::exceptions::shutdown const& s) {
    (void)s;
  }

  // Remove temporary file.
  QFile::remove(filename);

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
