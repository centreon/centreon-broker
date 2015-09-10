/*
** Copyright 2011-2014 Centreon
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

#include <cstdio>
#include <cstring>
#include <QDir>
#include <QFile>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_compression_stream_io.tmp"

/**
 *  Check that compression works properly.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Log on stderr.
  logging::file f(stderr);
  logging::manager::instance().log_on(
                                 f,
                                 logging::config_type
                                 | logging::debug_type
                                 | logging::error_type
                                 | logging::info_type,
                                 logging::low);

  // Generate file name.
  QString filename(QDir::tempPath());
  filename.append("/" TEMP_FILE_NAME);

  // Remove old file.
  ::remove(qPrintable(filename));

  // Generate data packet.
  misc::shared_ptr<io::raw> data(new io::raw);
  data->append("0123456789abcdefghijklmnopqrstuvwxyz");

  {
    // Open file for writing.
    misc::shared_ptr<file::stream>
      fs(new file::stream(filename.toStdString()));
    compression::stream cs(-1, 40000);
    cs.write_to(fs);

    // Write data in file.
    for (unsigned int i(0); i < 1000000; ++i)
      cs.write(data);
  }

  // Return value.
  int retval(0);

  // Check file size (check that compression worked).
  {
    QFile f(filename);
    qint64 size(f.size());
    retval = ((size <= 0) || (size >= 1000000 * data->size()));
  }

  {
    // Open file for reading.
    misc::shared_ptr<file::stream>
      fs(new file::stream(filename.toStdString()));
    compression::stream cs(-1);
    cs.read_from(fs);

    // Compare data read with data written.
    unsigned int bufferc(0);
    char const buffer[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    data.clear();
    unsigned int datac(0);
    for (unsigned int count(0); !retval && (count < 1000000);) {
      // Read data.
      if (data.isNull()) {
        misc::shared_ptr<io::data> d;
        cs.read(d);
        if (d.isNull()
            || (io::events::data_type<io::events::internal, 1>::value != d->type()))
          retval |= 1;
        else {
          data = d.staticCast<io::raw>();
          datac = 0;
        }
      }

      if (!retval) {
        // Compare data.
        unsigned int cb(((data->size() - datac)
                           < (sizeof(buffer) - 1 - bufferc))
                        ? (data->size() - datac)
                        : (sizeof(buffer) - 1 - bufferc));
        retval |= memcmp(data->QByteArray::data() + datac,
                         buffer + bufferc,
                         cb);

        // Adjust buffers.
        bufferc += cb;
        if (bufferc == (sizeof(buffer) - 1)) {
          ++count;
          bufferc = 0;
        }
        datac += cb;
        if (datac == static_cast<unsigned int>(data->size()))
          data.clear();
      }
    }
    // EOF must be reached.
    try {
      misc::shared_ptr<io::data> d;
      cs.read(d);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& s) {
      (void)s;
    }
  }

  // Remove temporary file.
  QFile::remove(filename);

  return (retval);
}
