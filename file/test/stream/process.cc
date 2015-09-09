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

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

#define TEMP_FILE_NAME "broker_file_stream_process"

/**
 *  Check that file stream process events as requested.
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

  // Return value.
  int retval(0);

  // I = true / O = true
  {
    // Open file stream.
    file::stream fs(filename.toStdString());

    // Checked method.
    fs.process(true, true);

    // Write data to file.
    fs.write(data);

    // Read data from file.
    misc::shared_ptr<io::data> d;
    fs.read(d);
  }

  // I = false / O = true
  {
    // Open file stream.
    file::stream fs(filename.toStdString());

    // Write data to file.
    fs.write(data);

    // Checked method.
    fs.process(false, true);

    // Write data to file.
    try {
      fs.write(data);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& e) {
      (void)e;
    }

    // Read data from file.
    try {
      misc::shared_ptr<io::data> d;
      fs.read(d);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& e) {
      (void)e;
    }
  }

  // I = true / O = false.
  {
    // Open file stream.
    file::stream fs(filename.toStdString());

    // Checked method.
    fs.process(true, false);

    // Write data to file.
    fs.write(data);

    // Read data from file.
    misc::shared_ptr<io::data> d;
    fs.read(d);
  }

  // I = false / O = false.
  {
    // Open file stream.
    file::stream fs(filename.toStdString());

    // Checked method.
    fs.process(false, false);

    // Write data to file.
    fs.write(data);

    // Read data from file.
    try {
      misc::shared_ptr<io::data> d;
      fs.read(d);
      retval |= 1;
    }
    catch (io::exceptions::shutdown const& e) {
      (void)e;
    }
  }

  // Remove temporary file.
  QFile::remove(filename);

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
