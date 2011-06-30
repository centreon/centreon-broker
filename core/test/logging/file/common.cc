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
#include "common.hh"

using namespace com::centreon::broker;

// Log messages.
#define MSG1 "my first normal message\n"
#define MSG2 "my second foobar longer message\n"
#define MSG3 "my third message is even longer than the second\n"
#define MSG4 "my fourth messages is finally the longest of all bazqux\n"

/**
 *  Check a file content.
 *
 *  @param[in] path    File path.
 *  @param[in] pattern Core content pattern.
 *
 *  @return true if all log file match pattern.
 */
bool check_content(QString const& path, QString const& pattern) {
  // Return value.
  bool retval(true);

  // Open log file.
  QFile f(path);
  if (f.open(QIODevice::ReadOnly)) {
    // First line.
    f.waitForReadyRead(-1);
    QByteArray line(f.readLine());

    // Content strings.
    char const* content[] = { MSG1, MSG2, MSG3, MSG4 };

    // Match lines.
    for (unsigned int i = 0;
         i < sizeof(content) / sizeof(*content);
         ++i) {
      // Read line.
      f.waitForReadyRead(-1);
      line = f.readLine();

      // Match line.
      QString my_pattern(pattern);
      my_pattern.replace("<MSG>", content[i]);
      retval &= (QRegExp(my_pattern).indexIn(line) != -1);
    }

    // Last line.
    f.waitForReadyRead(-1);
    line = f.readLine();
    f.waitForReadyRead(-1);
    f.readLine();
    retval &= (!line.isEmpty() && f.atEnd());
  }
  else
    retval = false;

  // Return check result.
  return (retval);
}

/**
 *  Get a temporary file path.
 *
 *  @return Temporary file path.
 */
QString temp_file_path() {
  QString file_path;
  file_path.append(QDir::tempPath());
  file_path.append("/");
  file_path.append("centreon_broker_unit_test");
  return (file_path);
}

/**
 *  Write log messages to a backend.
 *
 *  @param[out] b Backend to write to.
 */
void write_log_messages(logging::backend* b) {
  // First message.
  b->log_msg(MSG1,
    strlen(MSG1),
    logging::CONFIG,
    logging::HIGH);

  // Second message.
  b->log_msg(MSG2,
    strlen(MSG2),
    logging::DEBUG,
    logging::MEDIUM);

  // Third message.
  b->log_msg(MSG3,
    strlen(MSG3),
    logging::ERROR,
    logging::LOW);

  // Fourth message.
  b->log_msg(MSG4,
    strlen(MSG4),
    logging::INFO,
    logging::HIGH);

  return ;
}
