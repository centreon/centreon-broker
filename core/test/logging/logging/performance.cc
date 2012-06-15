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

#include <iostream>
#include <QCoreApplication>
#include <QDir>
#include <QTime>
#include <stdlib.h>
#include <time.h>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

#define MESSAGE "0123456789abcdefghijklmnopqrstuvwxyz"
#define MESSAGE_COUNT 100000
#define TEMP_FILE1 "broker_logging_logging_performance1"
#define TEMP_FILE2 "broker_logging_logging_performance2"
#define TEMP_FILE3 "broker_logging_logging_performance3"

/**
 *  Check logging performance (informational).
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  logging::manager::load();

  // Build filenames.
  QString filename1(QDir::tempPath());
  filename1.append("/" TEMP_FILE1);
  QString filename2(QDir::tempPath());
  filename2.append("/" TEMP_FILE2);
  QString filename3(QDir::tempPath());
  filename3.append("/" TEMP_FILE3);

  // Remove old files.
  QFile::remove(filename1);
  QFile::remove(filename2);
  QFile::remove(filename3);

  // Initialize file backend.
  logging::file::with_thread_id(false);
  logging::file::with_timestamp(false);
  logging::file backend1(filename1);
  logging::file backend2(filename2);
  logging::file backend3(filename3);

  // Add backends to logging list.
  logging::manager::instance().log_on(
    backend1,
    logging::debug_type | logging::error_type | logging::info_type,
    logging::high);
  logging::manager::instance().log_on(
    backend2,
    logging::config_type | logging::error_type,
    logging::medium);
  logging::manager::instance().log_on(
    backend3,
    logging::error_type,
    logging::low);

  // Initialize random number generator.
  srand(time(NULL));

  // Timer.
  QTime timer;
  timer.start();

  // Log messages.
  for (unsigned int i = 0; i < MESSAGE_COUNT; ++i) {
    // Logging level.
    logging::level l;
    switch (rand() % 3) {
     case 0:
      l = logging::low;
      break ;
     case 1:
      l = logging::medium;
      break ;
     default:
      l = logging::high;
      break ;
    }

    // Logging type.
    switch (rand() % 4) {
     case 0:
      logging::config(l) << MESSAGE << i;
      break ;
     case 1:
      logging::debug(l) << MESSAGE << i;
      break ;
     case 2:
      logging::error(l) << MESSAGE << i;
      break ;
     default:
      logging::info(l) << MESSAGE << i;
      break ;
    }
  }

  // Performance output.
  int elapsed(timer.elapsed());
  std::cout << MESSAGE_COUNT << " messages in " << elapsed << " ms - "
    << static_cast<double>(MESSAGE_COUNT) / elapsed << " msg/ms"
    << std::endl;

  // Remove temp files.
  QFile::remove(filename1);
  QFile::remove(filename2);
  QFile::remove(filename3);

  return (0);
}
