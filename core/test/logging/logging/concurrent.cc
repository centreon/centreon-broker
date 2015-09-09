/*
** Copyright 2011-2012 Centreon
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
#include <QRegExp>
#include <QString>
#include <QThreadPool>
#include <QtCore>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;

#define MESSAGE "0123456789abcdefghijklmnopqrstuvwxyz"
#define TEMP_FILE "broker_logging_logging_concurrent"
#define THREAD_COUNT 50
#define WRITE_COUNT 200

/**
 *  Log some messages.
 */
static void log_messages() {
  for (unsigned int i = 0; i < WRITE_COUNT; ++i)
    logging::error(logging::high) << MESSAGE;
  return ;
}

/**
 *  Check that logging is thread-safe.
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
