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
#include <QRegExp>
#include <QString>
#include <QThreadPool>
#include <QtCore>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

#define MESSAGE "0123456789abcdefghijklmnopqrstuvwxyz"
#define TEMP_FILE "centreon_broker_unit_test.tmp"
#define THREAD_COUNT 50
#define WRITE_COUNT 200

/**
 *  Log some messages.
 */
static void log_messages() {
  for (unsigned int i = 0; i < WRITE_COUNT; ++i)
    logging::error << logging::HIGH << MESSAGE;
  return ;
}

/**
 *  Check that logging is thread-safe.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  logging::manager::load();

  // Build filename.
  QString filename(QDir::tempPath());
  filename.append("/" TEMP_FILE);

  // Remove old file.
  QFile::remove(filename);

  // Initialize file backend.
  logging::file::with_timestamp(true);
  logging::file backend(filename);

  // Add backend to logging list.
  logging::manager::instance().log_on(backend);

  // Reserve threads.
  for (unsigned int i = 0; i < THREAD_COUNT; ++i)
    QThreadPool::globalInstance()->reserveThread();

  // Run tasks.
  for (unsigned int i = 0; i < THREAD_COUNT; ++i)
    QtConcurrent::run(log_messages);

  // Wait for tasks to finish.
  QThreadPool::globalInstance()->waitForDone();

  // Remove backend from logging.
  logging::manager::instance().log_on(backend, 0, logging::none);

  // Release threads.
  QThreadPool::globalInstance()->releaseThread();

  // Return value.
  int retval(0);

  // Message regexp.
  QRegExp regex(".*" MESSAGE ".*");

  // Read file.
  QFile f(filename);
  retval |= !f.open(QIODevice::ReadOnly);
  if (!retval) {
    f.readLine();
    for (unsigned int i = 0;
         !retval && (i < THREAD_COUNT * WRITE_COUNT);
         ++i)
      retval |= (regex.indexIn(f.readLine()) == -1);
    f.readLine();
    retval |= !f.readLine().isEmpty();
  }

  // Remove temp file.
  QFile::remove(filename);

  return (retval);
}
