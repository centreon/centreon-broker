/*
** Copyright 2011-2013 Merethis
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
    fs.write(data.staticCast<io::data>());

    // Read data from file.
    misc::shared_ptr<io::data> d;
    fs.read(d);
  }

  // I = false / O = true
  {
    // Open file stream.
    file::stream fs(filename.toStdString());

    // Write data to file.
    fs.write(data.staticCast<io::data>());

    // Checked method.
    fs.process(false, true);

    // Write data to file.
    try {
      fs.write(data.staticCast<io::data>());
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
    fs.write(data.staticCast<io::data>());

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
    fs.write(data.staticCast<io::data>());

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
